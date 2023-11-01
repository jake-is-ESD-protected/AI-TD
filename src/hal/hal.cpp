#include "hal.hpp"
#include "daisy_seed.h"
#include <math.h>

#include "../../libK/Utilities/Map.hpp"
#include "../../src/cli/cli.hpp"
#include "../../src/transientDSP/transientDSP.hpp"
#include "../../src/ui/ui.hpp"

using namespace daisy;
using namespace k;

static DaisySeed *pHw;

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
    KnobAttack.updateKnob(pHw->adc.GetFloat(0));
    KnobAttackTime.updateKnob(pHw->adc.GetFloat(1));
    KnobSustain.updateKnob(pHw->adc.GetFloat(2));
    KnobSustainTime.updateKnob(pHw->adc.GetFloat(3));
    transientDSPuiProcess();
}

void VisualCallback(void *data)
{
    BlueLed.Set(fabs(lastVarGainValue) * LED_DISPLAY_GAIN);
    BlueLed.Update();
}

void halInit(DaisySeed *_pHw)
{
    pHw = _pHw;
    pHw->Configure();
    pHw->Init(true); // ENABLE BOOST MODE
    pHw->SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_96KHZ);
    pHw->SetAudioBlockSize(1);

    DacHandle::Config config = pHw->dac.GetConfig();
    config.mode = DacHandle::Mode::POLLING;
    config.bitdepth = DacHandle::BitDepth::BITS_12;
    pHw->dac.Init(config);

    BlueLed.Init(seed::D26, false, sampleRate);
    ButtonA.Init(daisy::seed::D27, GPIO::Mode::INPUT, GPIO::Pull::PULLUP);

    AdcChannelConfig adcConfig[4];
    adcConfig[0].InitSingle(pHw->GetPin(15));
    adcConfig[1].InitSingle(pHw->GetPin(16));
    adcConfig[2].InitSingle(pHw->GetPin(17));
    adcConfig[3].InitSingle(pHw->GetPin(18));
    pHw->adc.Init(adcConfig, 4);

    pHw->adc.Start();

    transientDSPinit();

    pHw->StartAudio(AudioCallback);
}

void halVCAwrite(double value)
{
    pHw->dac.WriteValue(DacHandle::Channel::ONE, Map::mapClip(value, 1, 0, 483, 2344));
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
    pHw->SetLed(b);
}

bool halButtonRead()
{
    return ButtonA.Read();
}
