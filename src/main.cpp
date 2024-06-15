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

float testArrayLuka[13] = {0.6035, 0.0948, 0.1893, 0.1793, 0.2599, 0.3051, 0.086, 0.0073, 0.1627, 0.441, 0.9564, 0, 0.19999999999999996}; // 0.75779283 0.88444906
float testArrayKristof[13] = {0.3874, 0.0185, 0.1383, 1.0, 0.076, 0.091, 0.0953, 0.0121, 0.494, 0.0246, 0.8758, 0, 0.3475999999999999};   // 0.7210246, 0.3815809
float testArrayZeros[13] = {0};                                                                                                           //
float evalAFVector[13];

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
