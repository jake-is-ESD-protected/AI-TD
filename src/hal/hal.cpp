#include "hal.hpp"

#include "daisy_seed.h"
#include "daisysp.h"
#include <math.h>

#include "../../libK/Utilities/Map.hpp"
#include "../../src/transientDSP/transientDSP.hpp"
#include "../../src/ui/ui.hpp"

using namespace daisy;
using namespace daisysp;
using namespace k;

static DaisySeed hw;

static Led BlueLed;
static GPIO blue;
static Oscillator osc;
double oscScaledValue;

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{

    // write2VCA(fabs(in[0][0]));
    /*oscScaledValue = (osc.Process() + 1.0) / 2.0;
    BlueLed.Set(Map::mapClip(Map::mapSkew(oscScaledValue *
    KnobAttackTime.getValue(), 3) + KnobSustainTime.getValue(), 0, 1 ,0.08,
    0.6)); BlueLed.Update();*/
    // write2VCA(Map::mapSkew(Map::mapClip(oscScaledValue *
    // KnobAttackTime.getValue() + KnobSustainTime.getValue() ,0, 1, 0, 1), 5));

    processTransientDSP(in[0][0]);
}

void write2VCA(double value)
{
    hw.dac.WriteValue(DacHandle::Channel::ONE, Map::mapClip(value, 1, 0, 483, 2344));
    // hw.dac.WriteValue(DacHandle::Channel::ONE, -1861 * value + 2344);
}

void doHalStuff()
{
    KnobAttack.updateKnob(hw.adc.GetFloat(0));
    KnobAttackTime.updateKnob(hw.adc.GetFloat(1));
    KnobSustain.updateKnob(hw.adc.GetFloat(2));
    KnobSustainTime.updateKnob(hw.adc.GetFloat(3));

    osc.SetFreq(Map::mapClip(Map::mapSkew(KnobAttack.getValue(), 0.2), 0, 1, 0.1, 30));
}

void initHal()
{
    hw.Configure();
    hw.Init();
    hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_96KHZ);
    hw.SetAudioBlockSize(1);

    float sampleRate = hw.AudioSampleRate();
    osc.Init(sampleRate);
    osc.SetWaveform(Oscillator::WAVE_SIN);
    osc.SetAmp(1.0);
    osc.SetFreq(0.25);

    DacHandle::Config config = hw.dac.GetConfig();
    config.mode = DacHandle::Mode::POLLING;
    config.bitdepth = DacHandle::BitDepth::BITS_12;
    hw.dac.Init(config);

    BlueLed.Init(seed::D26, false, sampleRate);

    AdcChannelConfig adcConfig[4];
    adcConfig[0].InitSingle(hw.GetPin(15));
    adcConfig[1].InitSingle(hw.GetPin(16));
    adcConfig[2].InitSingle(hw.GetPin(17));
    adcConfig[3].InitSingle(hw.GetPin(18));
    hw.adc.Init(adcConfig, 4);

    hw.adc.Start();

    initTransientDSP();

    hw.StartAudio(AudioCallback);
}

void setLed(bool b)
{
    hw.SetLed(b);
}