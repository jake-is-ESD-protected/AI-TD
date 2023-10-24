#include "hal.hpp"

#include "daisy_seed.h"
#include "daisysp.h"

#include "../../libK/Utilities/Map.hpp"

using namespace daisy;
using namespace daisysp;
using namespace k;

static DaisySeed  hw;

static Led BlueLed;
static GPIO blue;
static Oscillator osc;
double oscScaledValue;

static void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
	//double input = in[0][0];

	//COPY A BUNCH OF SHIT FROM MOJO CODEBASE

	oscScaledValue = (osc.Process() + 1.0) / 2.0;
	BlueLed.Set(Map::mapClip(Map::mapSkew(oscScaledValue * hw.adc.GetFloat(1), 3) + hw.adc.GetFloat(3), 0, 1 ,0.08, 0.6));
	BlueLed.Update();
	write2VCA(Map::mapSkew(Map::mapClip(oscScaledValue * hw.adc.GetFloat(1) + hw.adc.GetFloat(3),0, 1, 0, 1), 5));
}

void write2VCA(double value)
{
	hw.dac.WriteValue(DacHandle::Channel::ONE, Map::mapClip(value, 1, 0, 483, 2344));
}

void doHalStuff()
{
    osc.SetFreq(Map::mapClip(Map::mapSkew(hw.adc.GetFloat(0), 0.2), 0, 1, 0.1, 30));
}

void initHal() 
{
    hw.Configure();
    hw.Init();
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

    BlueLed.Init(seed::D26,false,sampleRate);

    AdcChannelConfig adcConfig[4];
    adcConfig[0].InitSingle(hw.GetPin(15));
    adcConfig[1].InitSingle(hw.GetPin(16));
    adcConfig[2].InitSingle(hw.GetPin(17));
    adcConfig[3].InitSingle(hw.GetPin(18));
    hw.adc.Init(adcConfig, 4);

    hw.adc.Start();

    hw.StartAudio(AudioCallback);
}

void setLed(bool b)
{
    hw.SetLed(b);
}