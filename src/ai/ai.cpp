#include "RTNeural.h"
#include "daisy_seed.h"
#include "model.h"
#include <vector>

#define LOAD_WEIGHTS(SHAPE_IN, SHAPE_OUT, target, source, counter)                                                                                                                                                                                                                                                                                                                                             \
    do                                                                                                                                                                                                                                                                                                                                                                                                         \
    {                                                                                                                                                                                                                                                                                                                                                                                                          \
        uint32_t *sourcePointer = source + counter;                                                                                                                                                                                                                                                                                                                                                            \
        for (int i = 0; i < SHAPE_OUT; i++)                                                                                                                                                                                                                                                                                                                                                                    \
        {                                                                                                                                                                                                                                                                                                                                                                                                      \
            std::vector<float> row;                                                                                                                                                                                                                                                                                                                                                                            \
            for (int j = 0; j < SHAPE_IN; j++)                                                                                                                                                                                                                                                                                                                                                                 \
            {                                                                                                                                                                                                                                                                                                                                                                                                  \
                row.push_back(*(float *)&sourcePointer[i * SHAPE_IN + j]);                                                                                                                                                                                                                                                                                                                                     \
            }                                                                                                                                                                                                                                                                                                                                                                                                  \
            target.push_back(row);                                                                                                                                                                                                                                                                                                                                                                             \
        }                                                                                                                                                                                                                                                                                                                                                                                                      \
        counter += SHAPE_IN * SHAPE_OUT;                                                                                                                                                                                                                                                                                                                                                                       \
    } while (0)

#define LOAD_BIAS(SHAPE_OUT, target, source, counter)                                                                                                                                                                                                                                                                                                                                                          \
    do                                                                                                                                                                                                                                                                                                                                                                                                         \
    {                                                                                                                                                                                                                                                                                                                                                                                                          \
        uint32_t *sourcePointer = source + counter;                                                                                                                                                                                                                                                                                                                                                            \
        for (int i = 0; i < SHAPE_OUT; i++)                                                                                                                                                                                                                                                                                                                                                                    \
        {                                                                                                                                                                                                                                                                                                                                                                                                      \
            target[i] = *(float *)&sourcePointer[i];                                                                                                                                                                                                                                                                                                                                                           \
        }                                                                                                                                                                                                                                                                                                                                                                                                      \
        counter += SHAPE_OUT;                                                                                                                                                                                                                                                                                                                                                                                  \
    } while (0)

// mirror the TensorFlow model as static model here:

//clang-format off

RTNeural::ModelT<float, IN_SHAPE, IN_SHAPE, RTNeural::DenseT<float, IN_SHAPE, SHAPE_L_1>, RTNeural::ReLuActivationT<float, SHAPE_L_1>, RTNeural::DenseT<float, SHAPE_L_1, SHAPE_L_2>, RTNeural::ReLuActivationT<float, SHAPE_L_2>, RTNeural::DenseT<float, SHAPE_L_2, SHAPE_L_3>, RTNeural::ReLuActivationT<float, SHAPE_L_3>, RTNeural::DenseT<float, SHAPE_L_3, SHAPE_L_4>,
                 RTNeural::ReLuActivationT<float, SHAPE_L_4>, RTNeural::DenseT<float, SHAPE_L_4, SHAPE_L_5>, RTNeural::ReLuActivationT<float, SHAPE_L_5>, RTNeural::DenseT<float, SHAPE_L_5, SHAPE_L_6>>
    taunet;

//clang-format on

float ATTACK_T1 = 2;
float SUSTAIN_T1 = 2;

uint32_t DSY_QSPI_BSS weigthsQSPI[1000000];

// uint32_t DSY_QSPI_BSS weights_l1_buffer[SHAPE_L_1][IN_SHAPE];
// uint32_t DSY_QSPI_BSS bias_l1_buffer[SHAPE_L_1];

// uint32_t DSY_QSPI_BSS weights_l2_buffer[SHAPE_L_2][SHAPE_L_1];
// uint32_t DSY_QSPI_BSS bias_l2_buffer[SHAPE_L_2];

// uint32_t DSY_QSPI_BSS weights_l3_buffer[SHAPE_L_3][SHAPE_L_2];
// uint32_t DSY_QSPI_BSS bias_l3_buffer[SHAPE_L_3];

// uint32_t DSY_QSPI_BSS weights_l4_buffer[SHAPE_L_4][SHAPE_L_3];
// uint32_t DSY_QSPI_BSS bias_l4_buffer[SHAPE_L_4];

// uint32_t DSY_QSPI_BSS weights_l5_buffer[SHAPE_L_5][SHAPE_L_4];
// uint32_t DSY_QSPI_BSS bias_l5_buffer[SHAPE_L_5];

// uint32_t DSY_QSPI_BSS weights_l6_buffer[SHAPE_L_6][SHAPE_L_5];
// uint32_t DSY_QSPI_BSS bias_l6_buffer[SHAPE_L_6];

float bias_l1_buffer[SHAPE_L_1];
float bias_l2_buffer[SHAPE_L_2];
float __attribute__((section(".sdram_bss"))) bias_l3_buffer[SHAPE_L_3];
float __attribute__((section(".sdram_bss"))) bias_l4_buffer[SHAPE_L_4];
float __attribute__((section(".sdram_bss"))) bias_l5_buffer[SHAPE_L_5];
float __attribute__((section(".sdram_bss"))) bias_l6_buffer[SHAPE_L_6];

std::vector<std::vector<float>> weights_l1;
std::vector<std::vector<float>> weights_l2;
std::vector<std::vector<float>> weights_l3;
std::vector<std::vector<float>> weights_l4;
std::vector<std::vector<float>> weights_l5;
std::vector<std::vector<float>> weights_l6;

float probeA = 0;
float probeB = 0;
float probeC = 0;

void aiInit(void)
{
    auto &layer_1 = taunet.get<0>();
    auto &layer_2 = taunet.get<2>();
    auto &layer_3 = taunet.get<4>();
    auto &layer_4 = taunet.get<6>();
    auto &layer_5 = taunet.get<8>();
    auto &layer_6 = taunet.get<10>();

    uint32_t counterQSPILoading = 0;

    LOAD_WEIGHTS(IN_SHAPE, SHAPE_L_1, weights_l1, weigthsQSPI, counterQSPILoading);
    layer_1.setWeights(weights_l1);

    LOAD_BIAS(SHAPE_L_1, bias_l1_buffer, weigthsQSPI, counterQSPILoading);
    layer_1.setBias(bias_l1_buffer);
    //*(float *)&sourcePointer[i * SHAPE_IN + j]

    LOAD_WEIGHTS(SHAPE_L_1, SHAPE_L_2, weights_l2, weigthsQSPI, counterQSPILoading);
    layer_2.setWeights(weights_l2);

    LOAD_BIAS(SHAPE_L_2, bias_l2_buffer, weigthsQSPI, counterQSPILoading);
    layer_2.setBias((const float *)bias_l2_buffer);

    // memcpy(bias_l1_buffer, weigthsQSPI + counterQSPILoading, SHAPE_L_1 * 4);
    // counterQSPILoading += SHAPE_L_1;
    // layer_1.setBias((const float *)bias_l1_buffer);

    // LOAD_WEIGHTS(SHAPE_L_1, SHAPE_L_2, weights_l2, weights_l2_buffer);
    // layer_2.setWeights(weights_l2);
    // layer_2.setBias((const float*)bias_l2_buffer);

    // LOAD_WEIGHTS(SHAPE_L_2, SHAPE_L_3, weights_l3, weights_l3_buffer);
    // layer_3.setWeights(weights_l3);
    // layer_3.setBias((const float*)bias_l3_buffer);

    // LOAD_WEIGHTS(SHAPE_L_3, SHAPE_L_4, weights_l4, weights_l4_buffer);
    // layer_4.setWeights(weights_l4);
    // layer_4.setBias((const float*)bias_l4_buffer);

    // LOAD_WEIGHTS(SHAPE_L_4, SHAPE_L_5, weights_l5, weights_l5_buffer);
    // layer_5.setWeights(weights_l5);
    // layer_5.setBias((const float*)bias_l5_buffer);

    // LOAD_WEIGHTS(SHAPE_L_5, SHAPE_L_6, weights_l6, weights_l6_buffer);
    // layer_6.setWeights(weights_l6);
    // layer_6.setBias((const float*)bias_l6_buffer);

    probeB = bias_l2_buffer[0];
    // probeB = weights_l2[1][0];
    // probeC = weights_l2[SHAPE_L_2 - 1][SHAPE_L_1 - 1];

    // layer_2.setWeights(weights_l2);
    // layer_3.setWeights(weights_l3);
    // layer_4.setWeights(weights_l4);
    // layer_5.setWeights(weights_l5);
    // layer_6.setWeights(weights_l6);

    // layer_1.setBias(bias_l1);
    // layer_2.setBias(bias_l2);
    // layer_3.setBias(bias_l3);
    // layer_4.setBias(bias_l4);
    // layer_5.setBias(bias_l5);
    // layer_6.setBias(bias_l6);
    // taunet.reset();
}

void aiRun(float *input)
{
    taunet.forward((float *)input);
    ATTACK_T1 = taunet.getOutputs()[0];
    SUSTAIN_T1 = taunet.getOutputs()[1];
}

float aiGetATTACK_T1(void)
{
    return ATTACK_T1;
}

float aiGetSUSTAIN_T1(void)
{
    return SUSTAIN_T1;
}