#include "hal.hpp"
#include "daisy_seed.h"
#include <math.h>

#include "Utilities/Map.hpp"
#include "ai.h"
#include "cli.hpp"
#include "mem.hpp"
#include "transientDSP.hpp"
#include "ui.hpp"

extern "C"
{
#include "../af/af.h"
}

using namespace daisy;
using namespace k;

DaisySeed hw;

static Led BlueLed;
static Led RedLed;
static Led PurpleLed;
static Switch LeftButton;
static Switch RightButton;

static TimerHandle timerUI;
static TimerHandle timerVisual;

uint64_t uiProcessCounter = 0;
uint64_t visualProcessCounter = 0;

bool lastPurpleButtonState = false;

bool processAFFlag = false; // THIS FLAG GOES UP WHILE RECORDING AND PROCESSING AND DOWN ON FINISHED PREPROCESSING AND AI INFRENCING

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
    double adcAudioIn = hw.adc.GetFloat(4) - 0.5 * 2;
    LeftButton.Debounce();
    RightButton.Debounce();
    if (lastPurpleButtonState && !LeftButton.Pressed()) // ON RELEASE
    {
        calculateAFFlag = true;
        halStopAudio();
        hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_96KHZ);
    }
    if (!lastPurpleButtonState && LeftButton.Pressed()) // ON PRESSED
    {
        processAFFlag = true; // SET PROCESS FLAG FOR MAIN LOOP
        halStopAudio();
        hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
        halStartAudio();
        // resetBuffer(); //TODO: FIX THIS
    }
    if (LeftButton.Pressed() && processAFFlag) // PUT INTO BUFFER ON PRESSED
    {
        AFInCAppend(in[0][0]);
    }
    else
    {
        if (!processAFFlag)
            transientDSPprocess(in[0][0]);
    }
    lastPurpleButtonState = LeftButton.Pressed();

    if (uiProcessCounter == 3200)
    {
        KnobAttack.updateKnob(hw.adc.GetFloat(0), LeftButton.Pressed());
        KnobSustain.updateKnob(hw.adc.GetFloat(1), LeftButton.Pressed());

        if (processAFFlag && (KnobAttackTime.updateKnob(hw.adc.GetFloat(2), LeftButton.Pressed()) || KnobSustainTime.updateKnob(hw.adc.GetFloat(3), LeftButton.Pressed())))
        {
            // DISSRUPT AI SHIET BY CHANGING STUFF BACK I GUESS BASED ON IF AUDIO IS ON DO THIS N THAT
            // halStopAudio();
            /*BlueLed.Set(1.0);
            RedLed.Set(1.0);
            halLEDset(false);
            PurpleLed.Set(1.0);
            calculateAFFlag = false;
            processAFFlag = false;
            calculationsDoneFlag = false;*/
            // TODO: WE PROPABLY NEED A FLAG BASED SETTING / RESETTING SYSTEM FOR THIS
            // hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_96KHZ);
            // halStartAudio();
        }

        transientDSPuiProcess();
        uiProcessCounter = 0;
    }
    uiProcessCounter++;

    if (visualProcessCounter == 1600)
    {
        if (RightButton.Pressed())
        {
            BlueLed.Set(0);
            RedLed.Set(0.8);
        }
        else
        {
            BlueLed.Set(0.8);
            RedLed.Set(0);
        }
        PurpleLed.Set(LeftButton.Pressed() ? 1 : (fabs(lastVarGainValue) * LED_DISPLAY_GAIN)); // TUNE ME DADDY

        visualProcessCounter = 0;
    }
    visualProcessCounter++;

    RedLed.Update();
    BlueLed.Update();
    PurpleLed.Update();
}

void resetShiftLayer()
{
    KnobAttackTime.updateKnob(0.1, true);
    KnobSustainTime.updateKnob(0.1, true);
    KnobAttackTime.updateKnob(0.5, true);
    KnobSustainTime.updateKnob(0.5, true);
}

void UICallback(void *data)
{
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
    LeftButton.Init(seed::D3, sampleRate, Switch::Type::TYPE_MOMENTARY, Switch::Polarity::POLARITY_NORMAL, Switch::Pull::PULL_UP);
    RightButton.Init(seed::D4, sampleRate, Switch::Type::TYPE_MOMENTARY, Switch::Polarity::POLARITY_NORMAL, Switch::Pull::PULL_UP);

    AdcChannelConfig adcConfig[4];
    adcConfig[0].InitSingle(hw.GetPin(15));
    adcConfig[1].InitSingle(hw.GetPin(16));
    adcConfig[2].InitSingle(hw.GetPin(17));
    adcConfig[3].InitSingle(hw.GetPin(18));
    adcConfig[4].InitSingle(hw.GetPin(19));
    hw.adc.Init(adcConfig, 5);

    hw.adc.Start();

    // cliInit();
    transientDSPinit();
    halTimerInit();
    aiInit();
    initAf();
    resetBuffer();
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
    return RightButton.Pressed();
}

void halStartAudio()
{
    hw.StartAudio(AudioCallback);
}

void halStopAudio()
{
    hw.StopAudio();
}
