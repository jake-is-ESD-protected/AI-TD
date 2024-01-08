#include "daisy_seed.h"
#include "hal.hpp"
#include "transientDSP.hpp"
#include "ui.hpp"

using namespace daisy;

int main(void)
{
    halInit();

    for (;;)
    {
        System::Delay(50);
        halLEDset(true);
        System::Delay(50);
        halLEDset(false);
    }
}
