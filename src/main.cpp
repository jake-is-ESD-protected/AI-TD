#include "ai.h"
#include "daisy_seed.h"
#include "hal.hpp"
#include "transientDSP.hpp"
#include "ui.hpp"

using namespace daisy;

// float testArray[15] = {0.6826, 0.5251, 0.7008, 0.1174, 0.1029, 0.2575, 0.1431, 0.466, 0.0256, 0.0615, 0.0038, 0.2181, 0.8451};
float testArray2[15] = {0.5811, 0.0243, 0.3052, 0.1541, 0.0352, 0.4844, 0.5923, 0.1084, 0.0381, 0.0697, 0.0, 0.6255, 0.4053}; // 1.0291564  0.22823034
// float testArrayZeros[15] = {0};
float testReturnA = 0;
float testReturnS = 0;

int main(void)
{
    halInit();
    halLEDset(true);

    for (;;)
    {
        System::Delay(50);
        halLEDset(true);
        System::Delay(50);
        halLEDset(false);
        aiRun(testArray2);
        testReturnA = aiGetATTACK_T1();
        testReturnS = aiGetSUSTAIN_T1();

        // EVERYTHING IS INTERRUPT DRIVEN
    }
}
