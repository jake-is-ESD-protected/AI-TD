#include "hal.hpp"
#include "daisy_seed.h"
#include <math.h>

#include "Utilities/Map.hpp"
#include "cli.hpp"
#include "mem.hpp"
#include "transientDSP.hpp"
#include "ui.hpp"
#include "ai.hpp"

using namespace daisy;
using namespace k;

DaisySeed hw;

static Led BlueLed;
static Led RedLed;
static Led PurpleLed;
static GPIO LeftButton;
static GPIO RightButton;

static TimerHandle timerUI;
static TimerHandle timerVisual;

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
    transientDSPprocess(in[0][0]);
    if (RightButton.Read())
    {
        BlueLed.Set(KnobAttackTime.getValue());
        RedLed.Set(KnobSustainTime.getValue());
        PurpleLed.Set(fabs(lastVarGainValue) * LED_DISPLAY_GAIN);
    }
    else
    {
        BlueLed.Set(0);
        RedLed.Set(0);
        PurpleLed.Set(0);
    }
    RedLed.Update();
    BlueLed.Update();
    PurpleLed.Update();
}

void UICallback(void *data)
{
    KnobAttack.updateKnob(hw.adc.GetFloat(0));
    KnobAttackTime.updateKnob(hw.adc.GetFloat(2));
    KnobSustain.updateKnob(hw.adc.GetFloat(1));
    KnobSustainTime.updateKnob(hw.adc.GetFloat(3));
    transientDSPuiProcess();
}

void VisualCallback(void *data)
{
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

    BlueLed.Init(seed::D1, true, sampleRate);
    PurpleLed.Init(seed::D6, false, sampleRate);
    RedLed.Init(seed::D2, true, sampleRate);
    LeftButton.Init(daisy::seed::D3, GPIO::Mode::INPUT, GPIO::Pull::PULLUP);
    RightButton.Init(daisy::seed::D4, GPIO::Mode::INPUT, GPIO::Pull::PULLUP);

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
    aiInit();
    halStartAudio();
}

void halVCAwrite(double value)
{
    hw.dac.WriteValue(DacHandle::Channel::BOTH, Map::mapClip(value, 1, 0, 0, 4095));
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
    return RightButton.Read();
}

void halStartAudio()
{
    hw.StartAudio(AudioCallback);
}

void halStopAudio()
{
    hw.StopAudio();
}
