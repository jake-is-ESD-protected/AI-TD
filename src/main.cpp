#include "Utilities/Map.hpp"
#include "ai.h"
#include "daisy_seed.h"
#include "hal.hpp"
#include "transientDSP.hpp"
#include "ui.hpp"

extern "C"
{
#include "af.h"
}

using namespace daisy;
using namespace k;

float testArrayLuka[14] = {0.6793, 0.0421, 0.1622, 0.7628, 0.5817, 0.3864, 0.0514, 0.0175, 0.2437, 0.0, 0, 0.395, 0, 0.687};            // 0.31151158 0.78028417
float testArrayKristof[14] = {0.3874, 0.0185, 0.1383, 1.0, 0.076, 0.091, 0.0953, 0.0121, 0.494, 0.0246, 0.8758, 0, 0.3475999999999999}; // 0.7210246, 0.3815809
float testArrayZeros[14] = {0};                                                                                                         // 0.28777623 0.03888492
float evalAFVector[14];

int main(void)
{
    halInit();
    for (;;)
    {
        if (processAFFlag)
        {
            processBTT();             // PROCESS BTT SAMPLES WHILE RECORDING
            if (calculationsDoneFlag) // IF CALCULATION HAS HAPPEND IN ABOVE FUNCTION
            {
                processAFFlag = false;
                // TODO: MOVE ALL OF THIS TO SEPERATE FILE AI.C?
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

                aiRun(evalAFVector);
                aiAttack = aiGetATTACK_T1();
                aiSustain = aiGetSUSTAIN_T1();
                aiMode = true;
                // TODO: TRIGGER GLOW UP ANIM

                halStartAudio();
                calculationsDoneFlag = false;
                resetBuffer();
            }
        }
        if (cancelationFlag)
        {
            halStopAudio();
            hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_96KHZ);
            System::Delay(1);
            halStartAudio();
            resetBuffer();           // TODO: COMBINE THIS WITH BELOW FOR AI_RESET AND MOVE TO OTHER FILE
            calculateAFFlag = false; // clean up
            processAFFlag = false;
            calculationsDoneFlag = false;
            cancelationFlag = false;
        }
    }
}
