#include "daisy_seed.h"

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

static DaisySeed  hw;

double mapClip(double x, double in_min, double in_max, double out_min, double out_max) {
    double output = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    if(output > out_max)
        output = out_max;
    if(output < out_min)
        output = out_min;
    return output;
}

static void AudioCallback(const float *in, float *out, size_t size)
{

}

int main(void)
{
    hw.Configure();
    hw.Init();

	DacHandle::Config config = hw.dac.GetConfig();
    config.mode = DacHandle::Mode::POLLING;
    config.bitdepth = DacHandle::BitDepth::BITS_12;
    hw.dac.Init(config);

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
		runner += speed;
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
		hw.dac.WriteValue(DacHandle::Channel::ONE, mapClip(y_val + 1, 0, 2, 0, 4095));
 	}
}
