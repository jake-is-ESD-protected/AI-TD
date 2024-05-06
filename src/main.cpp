#include "ai.h"
#include "daisy_seed.h"
#include "hal.hpp"
#include "transientDSP.hpp"
#include "ui.hpp"

using namespace daisy;

double testArray[15] = {0.6826, 0.5251, 0.7008, 0.1174, 0.1029, 0.2575, 0.1431, 0.466, 0.0256, 0.0615, 0.0038, 0.2181, 0.8451};
double testReturnA = 0;
double testReturnS = 0;

int main(void)
{
    halInit();
    halLEDset(true);
    aiRun(testArray);
    testReturnA = aiGetATTACK_T1();
    testReturnS = aiGetSUSTAIN_T1();

    for (;;)
    {
        System::Delay(50);
        halLEDset(true);
        System::Delay(50);
        halLEDset(false);
        aiRun(testArray);
        testReturnA = aiGetATTACK_T1();
        testReturnS = aiGetSUSTAIN_T1();

        // EVERYTHING IS INTERRUPT DRIVEN
    }
}
