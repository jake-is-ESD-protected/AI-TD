#include "daisysp.h"
#include "daisy_seed.h"

#include "stm32h7xx_hal_rcc.h"

extern "C"
{
#include "ai_datatypes_defines.h"
#include "ai_platform.h"
#include "esc50v10.h"
#include "esc50v10_data.h"
}

#define CEQ_MAX				5

using namespace daisysp;
using namespace daisy;

static DaisySeed  hw;

static void AudioCallback(const float *in, float *out, size_t size)
{
}

int main(void)
{
    hw.Configure();
    hw.Init();

	uint8_t top3_class_idx[3] = {0};
	float top3_class_score[3] = {0};

	ai_float scale_buffer[AI_ESC50V10_IN_1_HEIGHT][AI_ESC50V10_IN_1_WIDTH] =
	{
		0//#include "ai_verify.txt"
	};

	ai_float out_data[AI_ESC50V10_OUT_1_SIZE];
	ai_float in_data[AI_ESC50V10_IN_1_HEIGHT][AI_ESC50V10_IN_1_WIDTH];
	ai_handle pNN = AI_HANDLE_NULL;
	ai_u8 activations[AI_ESC50V10_DATA_ACTIVATIONS_SIZE];
	ai_buffer *ai_input = NULL;
	ai_buffer *ai_output = NULL;
	ai_error err;
	ai_network_report report;
	ai_i32 n_batch;

	ai_float out_fifo[CEQ_MAX][AI_ESC50V10_OUT_1_SIZE] = {0};
	const ai_handle acts[] = { activations };
	__HAL_RCC_CRC_CLK_ENABLE();
	err = ai_esc50v10_create_and_init(&pNN, acts, NULL);
	if (err.type != AI_ERROR_NONE)
	{
		while(1) {
			hw.SetLed(true);
			System::Delay(100);
			hw.SetLed(false);
			System::Delay(100);
		}
	}

	ai_input = ai_esc50v10_inputs_get(pNN, NULL);
	ai_output = ai_esc50v10_outputs_get(pNN, NULL);

	ai_input[0].data = AI_HANDLE_PTR(in_data);
	ai_output[0].data = AI_HANDLE_PTR(out_data);

    // start callback
    hw.StartAudio(AudioCallback);

    while(1) {
		hw.SetLed(true);
		System::Delay(500);
		hw.SetLed(false);
		System::Delay(500);
	}
}
