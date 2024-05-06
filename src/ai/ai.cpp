#include "RTNeural.h"
#include "model.h"

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
    RTNeural::DenseT<float, SHAPE_L_5, SHAPE_L_6>,
    RTNeural::ReLuActivationT<float, SHAPE_L_6>,
    RTNeural::DenseT<float, SHAPE_L_6, SHAPE_L_7>> taunet;

static double ATTACK_T1 = 2;
static double SUSTAIN_T1 = 2;

void aiInit(void){
    auto& layer_1 = taunet.get<0>();
    auto& layer_2 = taunet.get<2>();
    auto& layer_3 = taunet.get<4>();
    auto& layer_4 = taunet.get<6>();
    auto& layer_5 = taunet.get<8>();
    auto& layer_6 = taunet.get<10>();
    auto& layer_7 = taunet.get<12>();

    layer_1.setWeights(weights_l1);
    /*layer_2.setWeights(weights_l2);
    layer_3.setWeights(weights_l3);
    layer_4.setWeights(weights_l4);
    layer_5.setWeights(weights_l5);
    layer_6.setWeights(weights_l6);
    layer_7.setWeights(weights_l7);

    layer_1.setBias(bias_l1);
    layer_2.setBias(bias_l2);
    layer_3.setBias(bias_l3);
    layer_4.setBias(bias_l4);
    layer_5.setBias(bias_l5);
    layer_6.setBias(bias_l6);
    layer_7.setBias(bias_l7);*/
}

void aiRun(double* input){
    taunet.reset();
    taunet.forward((float*)input);
    ATTACK_T1 = (double)taunet.getOutputs()[0];
    SUSTAIN_T1 = (double)taunet.getOutputs()[1];
}

double aiGetATTACK_T1(void) { return ATTACK_T1; }

double aiGetSUSTAIN_T1(void) { return SUSTAIN_T1; }

//ALLOCATE FAKE ARRAY OF AF
//CALL aiRun with the array as arg ()
//fetch

/*

0.6826, 0.5251, 0.7008, 0.1174, 0.1029, 0.2575, 0.1431, 0.466, 0.0256, 0.0615, 0.0038, 0.2181, 0.8451

df.columns = ["TEMPO",
              "T1A", 
              "T2A", 
              "CENTROID", 
              "FLATNESS", 
              "L", 
              "ML", 
              "MH", 
              "H", 
              "CREST",
              "FLUX",
              "ATTACK_GAIN",
              "SUSTAIN_GAIN",
              "ATTACK_T1",
              "SUSTAIN_T1"]
*/