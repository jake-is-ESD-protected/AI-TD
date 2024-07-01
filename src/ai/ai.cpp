#include "RTNeural.h"
#include "daisy_seed.h"
#include "model.h"
#include <vector>

#include "Utilities/Map.hpp"
#include "ai.h"
#include "daisy_seed.h"
#include "hal.hpp"
#include "stm32h7xx_hal.h"
#include "transientDSP.hpp"
#include "ui.hpp"

extern "C"
{
#include "af.h"
}

using namespace k;

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

// clang-format off
RTNeural::ModelT<float, IN_SHAPE, IN_SHAPE,
    RTNeural::DenseT<float, IN_SHAPE, SHAPE_L_1>,
    RTNeural::ReLuActivationT<float, SHAPE_L_1>,
    RTNeural::DenseT<float, SHAPE_L_1, SHAPE_L_2>,
    RTNeural::ReLuActivationT<float, SHAPE_L_2>,
    RTNeural::DenseT<float, SHAPE_L_2, SHAPE_L_3>,
    RTNeural::ReLuActivationT<float, SHAPE_L_3>,
    RTNeural::DenseT<float, SHAPE_L_3, SHAPE_L_4>,
    RTNeural::ReLuActivationT<float, SHAPE_L_4>,
    RTNeural::DenseT<float, SHAPE_L_4, SHAPE_L_5>,
    RTNeural::ReLuActivationT<float, SHAPE_L_5>,
    RTNeural::DenseT<float, SHAPE_L_5, SHAPE_L_6>>
taunet;
// clang-format on

float ATTACK_T1 = 0.5;
float SUSTAIN_T1 = 0.5;

float evalAFVector[14];

bool processAFFlag = false; // THIS FLAG GOES UP WHILE RECORDING AND PROCESSING AND DOWN ON FINISHED PREPROCESSING AND AI INFRENCING
bool aiMode = false;

float aiAttack = 0.5;
float aiSustain = 0.5;

#define QSPI_STORAGE_SPACE (IN_SHAPE * SHAPE_L_1 + SHAPE_L_1) + (SHAPE_L_1 * SHAPE_L_2 + SHAPE_L_2) + (SHAPE_L_2 * SHAPE_L_3 + SHAPE_L_3) + (SHAPE_L_3 * SHAPE_L_4 + SHAPE_L_4) + (SHAPE_L_4 * SHAPE_L_5 + SHAPE_L_5) + (SHAPE_L_5 * SHAPE_L_6 + SHAPE_L_6)
uint32_t DSY_QSPI_BSS weigthsQSPI[QSPI_STORAGE_SPACE];

uint32_t time_inf = 0;

float __attribute__((section(".sdram_bss"))) bias_l1_buffer[SHAPE_L_1];
float __attribute__((section(".sdram_bss"))) bias_l2_buffer[SHAPE_L_2];
float __attribute__((section(".sdram_bss"))) bias_l3_buffer[SHAPE_L_3];
float __attribute__((section(".sdram_bss"))) bias_l4_buffer[SHAPE_L_4];
float __attribute__((section(".sdram_bss"))) bias_l5_buffer[SHAPE_L_5];
float __attribute__((section(".sdram_bss"))) bias_l6_buffer[SHAPE_L_6];

std::vector<std::vector<float>> weights_l1_vec;
std::vector<std::vector<float>> weights_l2_vec;
std::vector<std::vector<float>> weights_l3_vec;
std::vector<std::vector<float>> weights_l4_vec;
std::vector<std::vector<float>> weights_l5_vec;
std::vector<std::vector<float>> weights_l6_vec;

void aiInit(void)
{
    auto &layer_1 = taunet.get<0>();
    auto &layer_2 = taunet.get<2>();
    auto &layer_3 = taunet.get<4>();
    auto &layer_4 = taunet.get<6>();
    auto &layer_5 = taunet.get<8>();
    auto &layer_6 = taunet.get<10>();

    uint32_t counterQSPILoading = 0;

    LOAD_WEIGHTS(IN_SHAPE, SHAPE_L_1, weights_l1_vec, weigthsQSPI, counterQSPILoading);
    layer_1.setWeights(weights_l1_vec);

    LOAD_BIAS(SHAPE_L_1, bias_l1_buffer, weigthsQSPI, counterQSPILoading);
    layer_1.setBias(bias_l1_buffer);

    LOAD_WEIGHTS(SHAPE_L_1, SHAPE_L_2, weights_l2_vec, weigthsQSPI, counterQSPILoading);
    layer_2.setWeights(weights_l2_vec);

    LOAD_BIAS(SHAPE_L_2, bias_l2_buffer, weigthsQSPI, counterQSPILoading);
    layer_2.setBias(bias_l2_buffer);

    LOAD_WEIGHTS(SHAPE_L_2, SHAPE_L_3, weights_l3_vec, weigthsQSPI, counterQSPILoading);
    layer_3.setWeights(weights_l3_vec);

    LOAD_BIAS(SHAPE_L_3, bias_l3_buffer, weigthsQSPI, counterQSPILoading);
    layer_3.setBias(bias_l3_buffer);

    LOAD_WEIGHTS(SHAPE_L_3, SHAPE_L_4, weights_l4_vec, weigthsQSPI, counterQSPILoading);
    layer_4.setWeights(weights_l4_vec);

    LOAD_BIAS(SHAPE_L_4, bias_l4_buffer, weigthsQSPI, counterQSPILoading);
    layer_4.setBias(bias_l4_buffer);

    LOAD_WEIGHTS(SHAPE_L_4, SHAPE_L_5, weights_l5_vec, weigthsQSPI, counterQSPILoading);
    layer_5.setWeights(weights_l5_vec);

    LOAD_BIAS(SHAPE_L_5, bias_l5_buffer, weigthsQSPI, counterQSPILoading);
    layer_5.setBias(bias_l5_buffer);

    LOAD_WEIGHTS(SHAPE_L_5, SHAPE_L_6, weights_l6_vec, weigthsQSPI, counterQSPILoading);
    layer_6.setWeights(weights_l6_vec);

    LOAD_BIAS(SHAPE_L_6, bias_l6_buffer, weigthsQSPI, counterQSPILoading);
    layer_6.setBias(bias_l6_buffer);

    taunet.reset();
}

// float testArrayLuka[14] = {0.6793, 0.0421, 0.1622, 0.7628, 0.5817, 0.3864, 0.0514, 0.0175, 0.2437, 0.0, 0, 0.395, 0, 0.687};            // 0.31151158 0.78028417
// float testArrayKristof[14] = {0.3874, 0.0185, 0.1383, 1.0, 0.076, 0.091, 0.0953, 0.0121, 0.494, 0.0246, 0.8758, 0, 0.3475999999999999}; // 0.7210246, 0.3815809
// float testArrayZeros[14] = {0};                                                                                                         // 0.28777623 0.03888492

void aiProcess()
{
    if (processAFFlag)
    {
        processBTT();             // PROCESS BTT SAMPLES WHILE RECORDING
        if (calculationsDoneFlag) // IF CALCULATION HAS HAPPEND IN ABOVE FUNCTION
        {
            processAFFlag = false;
            evalAFVector[0] = afGetTempo();            // Tempo
            evalAFVector[1] = afGetT1A();              // T1A
            evalAFVector[2] = afGetT2A();              // T2A
            evalAFVector[3] = afGetSpectralCentroid(); // Centroid
            evalAFVector[4] = afGetPBandL();           // EQ_L
            evalAFVector[5] = afGetPBandML();          // EQ_ML
            evalAFVector[6] = afGetPBandMH();          // EQ_MH
            evalAFVector[7] = afGetPBandH();           // EQ_H
            evalAFVector[8] = afGetCrestFactor();      // Crest
            evalAFVector[9] = afGetSpectralFlux();     // Flux

            if (KnobAttack.getValue() > 0.5) // BOOST
            {
                evalAFVector[10] = 0;                                 // Attack_cut
                evalAFVector[11] = (KnobAttack.getValue() - 0.5) * 2; // Attack_boost
            }
            else // CUT
            {
                evalAFVector[10] = Map::mapClip(KnobAttack.getValue(), 0, 0.5, 1, 0); // Attack_cut
                evalAFVector[11] = 0;                                                 // Attack_boost
            }

            if (KnobSustain.getValue() > 0.5) // BOOST
            {
                evalAFVector[12] = 0;                                  // Sustain_cut
                evalAFVector[13] = (KnobSustain.getValue() - 0.5) * 2; // Sustain_boost
            }
            else // CUT
            {
                evalAFVector[12] = Map::mapClip(KnobSustain.getValue(), 0, 0.5, 1, 0); // Sustain_cut
                evalAFVector[13] = 0;                                                  // Sustain_boost
            }

            uint32_t start_inf = HAL_GetTick();
            aiRun(evalAFVector);
            uint32_t stop_inf = HAL_GetTick();
            time_inf = stop_inf - start_inf;

            halStartAudio();
            aiReset();
        }
    }

    if (cancelationFlag) // FLAG BASED INSPECTION OF CANCELATION
    {
        halStopAudio();
        hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_96KHZ);
        System::Delay(1);
        halStartAudio();
        aiReset();
    }
}

void aiRun(float *input)
{
    taunet.forward((float *)input);
    aiAttack = taunet.getOutputs()[0];
    aiSustain = taunet.getOutputs()[1];
    aiMode = true;
}

void aiCancel()
{
    cancelationFlag = true;
    processAFFlag = false;
    aiMode = false;
}

void aiReset()
{
    resetBuffer();
    calculateAFFlag = false;
    processAFFlag = false;
    calculationsDoneFlag = false;
    cancelationFlag = false;
}

float aiGetATTACK_T1(void)
{
    return aiAttack;
}

float aiGetSUSTAIN_T1(void)
{
    return aiSustain;
}

uint32_t aiGetTimeInf(void)
{
    return time_inf;
}