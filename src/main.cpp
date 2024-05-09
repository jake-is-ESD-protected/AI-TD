#include "ai.h"
#include "daisy_seed.h"
#include "hal.hpp"
#include "transientDSP.hpp"
#include "ui.hpp"

using namespace daisy;

double testArray[15] = {0.6826, 0.5251, 0.7008, 0.1174, 0.1029, 0.2575, 0.1431, 0.466, 0.0256, 0.0615, 0.0038, 0.2181, 0.8451};
double testArray2[15] = {0.7748, 0.012, 0.2251, 0.1621, 0.1335, 0.4836, 0.5895, 0.1336, 0.0194, 0.1007, 0.0475, 0.6738, 0.8758}; // 0.7210246, 0.3815809
double testArrayZeros[15] = {0};
double testReturnA = 0;
double testReturnS = 0;

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
        aiRun(testArrayZeros);
        testReturnA = aiGetATTACK_T1();
        testReturnS = aiGetSUSTAIN_T1();

        // EVERYTHING IS INTERRUPT DRIVEN
    }
}
