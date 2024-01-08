#include "ai.hpp"
#include "hal.hpp"
#include "stm32h7xx_hal_rcc.h"

extern "C"
{
#include "ai_datatypes_defines.h"
#include "ai_platform.h"
#include "nn.h"
#include "nn_data.h"
}

ai_float out_data[AI_NN_OUT_1_SIZE];
ai_float in_data[AI_NN_IN_1_SIZE];
ai_handle pNN = AI_HANDLE_NULL;
ai_u8 activations[AI_NN_DATA_ACTIVATIONS_SIZE];
const ai_handle acts[] = {activations};
ai_buffer *ai_input = NULL;
ai_buffer *ai_output = NULL;
ai_error err;

void aiInit(void)
{
    __HAL_RCC_CRC_CLK_ENABLE();
    err = ai_nn_create_and_init(&pNN, acts, NULL);

    if (err.type != AI_ERROR_NONE)
    {
        while (1)
        {
            halLEDset(true);
        }
    }
}

void aiRun(void)
{
    ai_input = ai_nn_inputs_get(pNN, NULL);
    ai_output = ai_nn_outputs_get(pNN, NULL);

    ai_input[0].data = AI_HANDLE_PTR(in_data);
    ai_output[0].data = AI_HANDLE_PTR(out_data);

    ai_float y_val = 0;
    ai_i32 nbatch = 1;

    // Perform inference
    nbatch = ai_nn_run(pNN, &ai_input[0], &ai_output[0]);
    if (nbatch != 1)
    {
        while (1)
        {
            halLEDset(true);
        }
    }
    y_val = ((float *)out_data)[0];
}

void aiFillInput(int32_t *data)
{
    for (uint32_t i = 0; i < AI_NN_IN_1_SIZE; i++)
    {
        ((ai_float *)in_data)[i] = (ai_float)data[i];
    }
}