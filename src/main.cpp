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

// float testArray[15] = {0.6826, 0.5251, 0.7008, 0.1174, 0.1029, 0.2575, 0.1431, 0.466, 0.0256, 0.0615, 0.0038, 0.2181, 0.8451};
float testArrayLuka[15] = {0.9566, 0.0362, 0.0996, 0.6371, 0.8114, 0.4654, 0.0797, 0.0068, 0.1171, 0.5104, 0.2, 0, 0.4838}; // 0.4915142, 0.05078933333333333
float testArrayZeros[15] = {0};                                                                                             //[0.25394556 0.33098203]
float testReturnA = 0;
float testReturnS = 0;

float afProbe = 0;

int main(void)
{
    halInit();
    halLEDset(true);

    for (;;)
    {
        System::Delay(10);
        aiRun(testArrayZeros);
        testReturnA = aiGetATTACK_T1();
        testReturnS = aiGetSUSTAIN_T1();
        System::Delay(10);
        if (processAFFlag)
        {
            processBTT();             // PROCESS BTT SAMPLES WHILE RECORDING
            if (calculationsDoneFlag) // IF CALCULATION HAS HAPPEND IN ABOVE FUNCTION
            {
                calculationsDoneFlag = false;
                processAFFlag = false;
                afProbe = afGetTempo();

                /*aiRun(testArray2);
                testReturnA = aiGetATTACK_T1();
                testReturnS = aiGetSUSTAIN_T1();*/

                halStartAudio();
            }
        }
        if (cancelationFlag)
        {
            halStopAudio();
            hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_96KHZ);
            System::Delay(1);
            halStartAudio();
            calculateAFFlag = false; // clean up
            processAFFlag = false;
            calculationsDoneFlag = false;
            cancelationFlag = false;
        }
    }
}
