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

float testArrayLuka[15] = {0.9566, 0.1498, 0.1552, 0.8348, 0.4552, 0.482, 0.1599, 0.0237, 0.2225, 0.4043, 0.0, 0, 0.7678}; // 0.39971474 0.42938083
float testArrayZeros[15] = {0};                                                                                            // 0.25394556 0.33098203
float evalAFVector[13];

int main(void)
{
    halInit();
    // halLEDset(true);

    for (;;)
    {
        if (processAFFlag)
        {
            processBTT();             // PROCESS BTT SAMPLES WHILE RECORDING
            if (calculationsDoneFlag) // IF CALCULATION HAS HAPPEND IN ABOVE FUNCTION
            {
                // halLEDset(false);
                calculationsDoneFlag = false;
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
                evalAFVector[10] = KnobSustain.getValue(); // Sustain_gain
                if (KnobAttack.getValue() > 0.5)           // BOOST
                {
                    evalAFVector[11] = 0;                                 // Attack_cut
                    evalAFVector[12] = (KnobAttack.getValue() - 0.5) * 2; // Attack_boost
                }
                else // CUT
                {
                    evalAFVector[11] = Map::mapClip(KnobAttack.getValue(), 0, 0.5, 1, 0); // Attack_cut
                    evalAFVector[12] = 0;                                                 // Attack_boost
                }

                aiRun(evalAFVector);
                aiAttack = aiGetATTACK_T1();
                aiSustain = aiGetSUSTAIN_T1();
                aiMode = true;
                // TODO: TRIGGER GLOW UP ANIM

                halStartAudio();
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
