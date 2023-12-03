#include "hal.hpp"
#include "daisy_seed.h"
#include <math.h>

#include "Utilities/Map.hpp"
#include "cli.hpp"
#include "mem.hpp"
#include "transientDSP.hpp"
#include "ui.hpp"

using namespace daisy;
using namespace k;

static DaisySeed hw;

static Led BlueLed;
static GPIO ButtonA;

static TimerHandle timerUI;
static TimerHandle timerVisual;

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
    transientDSPprocess(in[0][0]);
}

void UICallback(void *data)
{
    KnobAttack.updateKnob(hw.adc.GetFloat(0));
    KnobAttackTime.updateKnob(hw.adc.GetFloat(1));
    KnobSustain.updateKnob(hw.adc.GetFloat(2));
    KnobSustainTime.updateKnob(hw.adc.GetFloat(3));
    transientDSPuiProcess();
}

void VisualCallback(void *data)
{
    BlueLed.Set(fabs(lastVarGainValue) * LED_DISPLAY_GAIN);
    BlueLed.Update();
}

void halInit()
{
    hw.Configure();
    hw.Init(false); // ENABLE BOOST MODE
    hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_96KHZ);
    hw.SetAudioBlockSize(1);

    DacHandle::Config config = hw.dac.GetConfig();
    config.mode = DacHandle::Mode::POLLING;
    config.bitdepth = DacHandle::BitDepth::BITS_12;
    hw.dac.Init(config);

    BlueLed.Init(seed::D26, false, sampleRate);
    ButtonA.Init(daisy::seed::D27, GPIO::Mode::INPUT, GPIO::Pull::PULLUP);

    AdcChannelConfig adcConfig[4];
    adcConfig[0].InitSingle(hw.GetPin(15));
    adcConfig[1].InitSingle(hw.GetPin(16));
    adcConfig[2].InitSingle(hw.GetPin(17));
    adcConfig[3].InitSingle(hw.GetPin(18));
    hw.adc.Init(adcConfig, 4);

    hw.adc.Start();

    cliInit();
    transientDSPinit();
    halTimerInit();
    // halStartAudio();
}

void halVCAwrite(double value)
{
    hw.dac.WriteValue(DacHandle::Channel::ONE, Map::mapClip(value, 1, 0, 483, 2344));
}

void halTimerInit()
{
    TimerHandle::Config timcfg;
    timcfg.periph = daisy::TimerHandle::Config::Peripheral::TIM_5;
    timcfg.dir = daisy::TimerHandle::Config::CounterDir::UP;
    uint32_t tim_base_freq = daisy::System::GetPClk2Freq();
    unsigned long tim_period = tim_base_freq / 5;
    timcfg.period = tim_period;
    timcfg.enable_irq = true;
    timerUI.Init(timcfg);
    timerUI.SetCallback(UICallback, nullptr);
    timerUI.Start();

    TimerHandle::Config timcfgB;
    timcfgB.periph = daisy::TimerHandle::Config::Peripheral::TIM_4;
    timcfgB.dir = daisy::TimerHandle::Config::CounterDir::UP;
    unsigned long tim_periodB = tim_base_freq / 30;
    timcfgB.period = tim_periodB;
    timcfgB.enable_irq = true;
    timerVisual.Init(timcfgB);
    timerVisual.SetCallback(VisualCallback, nullptr);
    timerVisual.Start();
}

void halLEDset(bool b)
{
    hw.SetLed(b);
}

bool halButtonRead()
{
    return ButtonA.Read();
}

void halStartAudio()
{
    hw.StartAudio(AudioCallback);
}

void halStopAudio()
{
    hw.StopAudio();
}

void halUsbCdcInit(void (*cb)(uint8_t *buf, uint32_t *len))
{
    hw.usb_handle.Init(UsbHandle::FS_INTERNAL);
    System::Delay(1000);
    hw.usb_handle.SetReceiveCallback(cb, UsbHandle::FS_INTERNAL);
    System::Delay(1000);
}

void halUsbCdcTransmit(uint8_t *buf, uint32_t len)
{
    hw.usb_handle.TransmitInternal(buf, len);
}

void halEraseQspiFlash(uint8_t *mem, uint32_t len)
{
    uint32_t startAddr = (uint32_t)mem;
    uint32_t padLen = ((len + QSPI_FLASH_PAGE_SIZE - 1) / QSPI_FLASH_PAGE_SIZE) * QSPI_FLASH_PAGE_SIZE;
    hw.qspi.Erase(startAddr, startAddr + padLen);
}

void halWriteQspiFlash(uint32_t start, uint32_t len, uint8_t *content)
{
    uint32_t padLen = ((len + QSPI_FLASH_PAGE_SIZE - 1) / QSPI_FLASH_PAGE_SIZE) * QSPI_FLASH_PAGE_SIZE;
    hw.qspi.Write(start, padLen, content);
}