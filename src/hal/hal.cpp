#include "hal.hpp"
#include "daisy_seed.h"
#include <math.h>

#include "Utilities/Map.hpp."
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

float lastKnobPos[4] = {0.5, 0.5, 0.5, 0.5}; // A1 A2 T1 T2

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
    float adcAudioIn = hw.adc.GetFloat(4) - 0.5 * 2;

    if (lastPurpleButtonState && !LeftButton.Pressed()) // ON PURPLE RELEASE
    {
        if (processAFFlag) // ONLY IF ON RELEASE AND NO CANCEL
        {
            calculateAFFlag = true;
            halStopAudio();
            hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_96KHZ);
        }
    }
    if (!lastPurpleButtonState && LeftButton.Pressed()) // ON PURPLE PRESSED
    {
        processAFFlag = true;                // SET PROCESS FLAG FOR MAIN LOOP
        lastKnobPos[0] = hw.adc.GetFloat(0); // REMEMBER KNOB POSITIONS FOR CANCEL INTERACTION
        lastKnobPos[1] = hw.adc.GetFloat(1);
        lastKnobPos[2] = hw.adc.GetFloat(2);
        lastKnobPos[3] = hw.adc.GetFloat(3);
        halStopAudio();
        hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
        halStartAudio();
    }
    // CHECK WHICH TIME CONSTANTS ARE BEING USED
    if (aiMode)
    {
        if (fabs(lastKnobPos[2] - hw.adc.GetFloat(2)) > KNOB_WINDOW || fabs(lastKnobPos[3] - hw.adc.GetFloat(3)) > KNOB_WINDOW)
        {
            // LEAVE AI Mode
            aiMode = false;
        }
    }
    if (LeftButton.Pressed() && processAFFlag) // PUT INTO BUFFER ON PURPLE PRESSED
    {
        halVCAwrite(AI_LISTENING_UX_GAIN);
        AFInCAppend(in[0][0]);
    }
    else
    {
        if (!processAFFlag)
        {
            transientDSPprocess(in[0][0]);
        }
        else
        {
            halVCAwrite(AI_LISTENING_UX_GAIN);
        }
    }
    lastPurpleButtonState = LeftButton.Pressed();
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

const float VIS_STARTUP_TIME = sampleRateUIUX * 3; // 600=1s
const float VIS_GLOW_TIME = sampleRateUIUX * 6;    // 600=1s
uint32_t startUpTimer = 0;
uint32_t glowUpTimer = 0;

void VisualCallback(void *data)
{
    LeftButton.Debounce();
    RightButton.Debounce();

    if (processAFFlag && (fabs(lastKnobPos[0] - hw.adc.GetFloat(0)) > KNOB_WINDOW || fabs(lastKnobPos[1] - hw.adc.GetFloat(1)) > KNOB_WINDOW || fabs(lastKnobPos[2] - hw.adc.GetFloat(2)) > KNOB_WINDOW || fabs(lastKnobPos[3] - hw.adc.GetFloat(3)) > KNOB_WINDOW))
    { // SCAN FOR CANCEL
        aiCancel();
    }

    if (startUpTimer < VIS_STARTUP_TIME)
    {
        float fadeVar = 0;
        float currentNormalizedTime = ((float)startUpTimer) / ((float)VIS_STARTUP_TIME);
        if (startUpTimer < VIS_STARTUP_TIME * 0.333333)
        {
            fadeVar = Map::mapSkew(currentNormalizedTime * 3, 1.5); // GLOW UP
            BlueLed.Set(0);
            RedLed.Set(0);
        }
        else if (startUpTimer < VIS_STARTUP_TIME * 0.6666666)
        {
            fadeVar = Map::mapSkew(Map::mapClip((currentNormalizedTime - 0.333333) * 3, 1, 0, 0, 1), 2.4); // GLOW DOWN; // HOLD
            if (RightButton.Pressed())
            {
                BlueLed.Set(0);
                RedLed.Set(Map::mapSkew((currentNormalizedTime - 0.333333) * 3 * 0.8, 1.8)); // GLOW UP
            }
            else
            {
                BlueLed.Set(Map::mapSkew((currentNormalizedTime - 0.333333) * 3 * 0.8, 1.8)); // GLOW UP
                RedLed.Set(0);
            }
        }
        else if (startUpTimer < VIS_STARTUP_TIME)
        {
            fadeVar = Map::mapSkew(Map::mapClip((currentNormalizedTime - 0.333333) * 3, 1, 0, 0, 1), 2.4); // GLOW DOWN; // HOLD
            // Map::mapSkew(Map::mapClip((currentNormalizedTime - 0.666666) * 3, 1, 0, 0, 1), 2.1); // GLOW DOWN
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
        }
        PurpleLed.Set(fadeVar);
        startUpTimer++;
    }
    else // NOT STARTUP
    {
        if (glowUpTimer > VIS_GLOW_TIME)
            glowUpTimer = 0;
        KnobAttack.updateKnob(hw.adc.GetFloat(0), LeftButton.Pressed());
        KnobSustain.updateKnob(hw.adc.GetFloat(1), LeftButton.Pressed());
        KnobAttackTime.updateKnob(hw.adc.GetFloat(2), LeftButton.Pressed());
        KnobSustainTime.updateKnob(hw.adc.GetFloat(3), LeftButton.Pressed());

        transientDSPuiProcess();
        // UI TILL HERE
        // VISUAL FROM HERE

        if (LeftButton.Pressed())
        {
            if (processAFFlag)
            {
                // AI ANIM
                float retVal = Map::mapSkew((sin(2 * M_PI * glowUpTimer * 8 * (1.0 / VIS_GLOW_TIME)) + 2) / 2.0, 1) * 0.6; // IMPROVEMENT: MOVE THIS TO NICER VERSION
                BlueLed.Set(0.4 + retVal);
                RedLed.Set(0.4 + retVal);
            }
            else // AI CANCELD
            {
                BlueLed.Set(0.0);
                RedLed.Set(0.9);
                PurpleLed.Set(1);
            }
        }
        else
        {
            // REGULAR MODE OR CALC MODE
            if (calculateAFFlag || calculationsDoneFlag) // CALC MODE
            {
                float temp2 = Map::mapSkew((sin(2 * M_PI * glowUpTimer * 16 * (1.0 / VIS_GLOW_TIME)) + 2) / 2.0, 1) * 0.6; // IMPROVEMENT: MOVE THIS TO NICER VERSION
                BlueLed.Set(0.4 + temp2);
                RedLed.Set(0.4 + temp2);
            }
            else // REGULAR MODE
            {
                if (RightButton.Pressed())
                {
                    if (aiMode)
                    {
                        float temp2 = Map::mapSkew((sin(2 * M_PI * glowUpTimer * (1.0 / VIS_GLOW_TIME)) + 2) / 2.0, 1) * 0.3; // IMPROVEMENT: MOVE THIS TO NICER VERSION
                        BlueLed.Set(0.55 + temp2);
                        RedLed.Set(0.55 + temp2);
                    }
                    else
                    {
                        float temp2 = Map::mapSkew((sin(2 * M_PI * glowUpTimer * (1.0 / VIS_GLOW_TIME)) + 2) / 2.0, 1) * 0.3; // IMPROVEMENT: MOVE THIS TO NICER VERSION
                        BlueLed.Set(0);
                        RedLed.Set(0.55 + temp2);
                    }
                }
                else
                {
                    aiMode = false;
                    BlueLed.Set(0.8);
                    RedLed.Set(0);
                }
            }
            PurpleLed.Set(LeftButton.Pressed() ? 1 : (fabs(lastVarGainValue) * LED_DISPLAY_GAIN));
        }
        glowUpTimer++;
    }
    RedLed.Update();
    BlueLed.Update();
    PurpleLed.Update();
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

    BlueLed.Init(seed::D1, true, sampleRateUIUX);
    PurpleLed.Init(seed::D6, false, sampleRateUIUX);
    RedLed.Init(seed::D2, true, sampleRateUIUX);
    BlueLed.Set(0);
    RedLed.Set(0);
    RedLed.Update();
    BlueLed.Update();
    LeftButton.Init(seed::D3, sampleRateUIUX, Switch::Type::TYPE_MOMENTARY, Switch::Polarity::POLARITY_NORMAL, Switch::Pull::PULL_UP);
    RightButton.Init(seed::D4, sampleRateUIUX, Switch::Type::TYPE_MOMENTARY, Switch::Polarity::POLARITY_NORMAL, Switch::Pull::PULL_UP);

    AdcChannelConfig adcConfig[4]; // THIS SEEMS WRONG
    adcConfig[0].InitSingle(hw.GetPin(15));
    adcConfig[1].InitSingle(hw.GetPin(16));
    adcConfig[2].InitSingle(hw.GetPin(17));
    adcConfig[3].InitSingle(hw.GetPin(18));
    adcConfig[4].InitSingle(hw.GetPin(19));
    hw.adc.Init(adcConfig, 5);

    hw.adc.Start();
    halTimerInit();
    cliInit();
    transientDSPinit();
    aiInit();
    initAf();
    resetBuffer();
    halStartAudio();
}

void halVCAwrite(float value)
{
    hw.dac.WriteValue(DacHandle::Channel::BOTH, Map::mapClip(value, 1, 0, 0, 4095));
}

void halTimerInit()
{
    /*TimerHandle::Config timcfg;
    timcfg.periph = daisy::TimerHandle::Config::Peripheral::TIM_5;
    timcfg.dir = daisy::TimerHandle::Config::CounterDir::UP;
    uint32_t tim_base_freq = daisy::System::GetPClk2Freq();
    unsigned long tim_period = tim_base_freq / 5;
    timcfg.period = tim_period;
    timcfg.enable_irq = true;
    timerUI.Init(timcfg);
    timerUI.SetCallback(UICallback, nullptr);
    timerUI.Start();*/

    TimerHandle::Config timcfgB;
    timcfgB.periph = daisy::TimerHandle::Config::Peripheral::TIM_5;
    timcfgB.dir = daisy::TimerHandle::Config::CounterDir::UP;
    uint32_t tim_base_freq = daisy::System::GetPClk2Freq();
    unsigned long tim_periodB = tim_base_freq / sampleRateUIUX; // per second
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
