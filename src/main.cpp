#include "daisy_seed.h"
#include "daisysp.h"
#include "../../libK/Utilities/Map.hpp"

#include "stm32h7xx_hal_rcc.h"

extern "C"
{
#include "ai_datatypes_defines.h"
#include "ai_platform.h"
#include "nn.h"
#include "nn_data.h"
}

#define CEQ_MAX				5

using namespace daisy;
using namespace daisy;
using namespace daisysp;
using namespace k;

static DaisySeed  hw;
static Led BlueLed;
static GPIO blue;
static Oscillator osc;

double oscScaledValue;

void write2VCA(double value)
{
	hw.dac.WriteValue(DacHandle::Channel::ONE, Map::mapClip(value, 1, 0, 483, 2344));
}

static void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
	//double input = in[0][0];

	//COPY A BUNCH OF SHIT FROM MOJO CODEBASE

	oscScaledValue = (osc.Process() + 1.0) / 2.0;
	BlueLed.Set(Map::mapClip(Map::mapSkew(oscScaledValue * hw.adc.GetFloat(1), 3) + hw.adc.GetFloat(3), 0, 1 ,0.08, 0.6));
	BlueLed.Update();
	write2VCA(Map::mapSkew(Map::mapClip(oscScaledValue * hw.adc.GetFloat(1) + hw.adc.GetFloat(3),0, 1, 0, 1), 5));
}

int main(void)
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

	ai_float out_data[AI_NN_OUT_1_SIZE];
	ai_float in_data[AI_NN_IN_1_SIZE];
	ai_handle pNN = AI_HANDLE_NULL;
	ai_u8 activations[AI_NN_DATA_ACTIVATIONS_SIZE];
	ai_buffer *ai_input = NULL;
	ai_buffer *ai_output = NULL;
	ai_error err;

	const ai_handle acts[] = { activations };
	__HAL_RCC_CRC_CLK_ENABLE();
	err = ai_nn_create_and_init(&pNN, acts, NULL);
    
	if (err.type != AI_ERROR_NONE)
	{
		while(1) {
			hw.SetLed(true);
			System::Delay(500);
			hw.SetLed(false);
			System::Delay(500);
		}
	}
    
	ai_input = ai_nn_inputs_get(pNN, NULL);
	ai_output = ai_nn_outputs_get(pNN, NULL);

	ai_input[0].data = AI_HANDLE_PTR(in_data);
	ai_output[0].data = AI_HANDLE_PTR(out_data);

	double runner = 0;
	double speed = 0.01;
	double wraparound = 2 * 3.14;
	ai_float y_val = 0;
	ai_i32 nbatch = 1;

    for(;;)
	{
		if(0)
		System::Delay(10);
		osc.SetFreq(Map::mapClip(Map::mapSkew(hw.adc.GetFloat(0), 0.2), 0, 1, 0.1, 30));
		/*runner += speed;
		if(runner > wraparound)
			runner = 0;

		// Fill input buffer (use test value)
		for (uint32_t i = 0; i < AI_NN_IN_1_SIZE; i++)
		{
		((ai_float *)in_data)[i] = (ai_float) runner;
		}

		// Perform inference
		nbatch = ai_nn_run(pNN, &ai_input[0], &ai_output[0]);
		if (nbatch != 1) {
			while(1) {
				hw.SetLed(true);
				System::Delay(500);
				hw.SetLed(false);
				System::Delay(500);
			}
		}

		// Read output (predicted y) of neural network
		y_val = ((float *)out_data)[0];
		hw.dac.WriteValue(DacHandle::Channel::ONE, Map::mapClip(y_val + 1, 0, 2, 0, 4095));*/
 	}
}
