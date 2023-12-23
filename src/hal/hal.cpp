#include "hal.hpp"
#include "daisy_seed.h"
#include <math.h>

#include "../../libK/Utilities/Map.hpp"
#include "../../src/cli/cli.hpp"
#include "../../src/transientDSP/transientDSP.hpp"
#include "../../src/ui/ui.hpp"

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
    hw.Init(true); // ENABLE BOOST MODE
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

    cliInit(&hw);
    transientDSPinit();
    halTimerInit();

    hw.StartAudio(AudioCallback);
}

void halVCAwrite(double value)
{
    hw.dac.WriteValue(DacHandle::Channel::BOTH, Map::mapClip(KnobSustainTime.getValue(), 1, 0, 0, 4095));
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
