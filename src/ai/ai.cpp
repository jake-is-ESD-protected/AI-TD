#include "RTNeural.h"
#include "daisy_seed.h"
#include "model.h"
#include <vector>

// clang-format off
// mirror the TensorFlow model as static model here:

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
    RTNeural::DenseT<float, SHAPE_L_5, SHAPE_L_6>> taunet;

float ATTACK_T1 = 2;
float SUSTAIN_T1 = 2;

float DSY_QSPI_BSS weights_l1QSPIFlash[SHAPE_L_1][IN_SHAPE];

std::vector<std::vector<float>> weights_l1;

float probeA = 0;
float probeB = 0;
float probeC = 0;

void aiInit(void){
    auto& layer_1 = taunet.get<0>();
    auto& layer_2 = taunet.get<2>();
    auto& layer_3 = taunet.get<4>();
    auto& layer_4 = taunet.get<6>();
    auto& layer_5 = taunet.get<8>();
    auto& layer_6 = taunet.get<10>();

    for(int i = 0; i < SHAPE_L_1;i++) //load weights_l1 from flash
    {
        std::vector<float> row;
        for(int j = 0; j < IN_SHAPE;j++)
        {
            row.push_back(weights_l1QSPIFlash[i][j]);
        }
        weights_l1.push_back(row); 
    }

    layer_1.setWeights(weights_l1);
    probeA = weights_l1[0][0];
    probeB = weights_l1[0][1];
    probeC = weights_l1[SHAPE_L_1-1][IN_SHAPE-1];
    
    /*
    layer_2.setWeights(weights_l2);
    layer_3.setWeights(weights_l3);
    layer_4.setWeights(weights_l4);
    layer_5.setWeights(weights_l5);
    layer_6.setWeights(weights_l6);

    layer_1.setBias(bias_l1);
    layer_2.setBias(bias_l2);
    layer_3.setBias(bias_l3);
    layer_4.setBias(bias_l4);
    layer_5.setBias(bias_l5);
    layer_6.setBias(bias_l6);
    taunet.reset();*/
}

void aiRun(float* input){
    taunet.forward((float*)input);
    ATTACK_T1 = taunet.getOutputs()[0];
    SUSTAIN_T1 = taunet.getOutputs()[1];
}

float aiGetATTACK_T1(void) { return ATTACK_T1; }

float aiGetSUSTAIN_T1(void) { return SUSTAIN_T1; }