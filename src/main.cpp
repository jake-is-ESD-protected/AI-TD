#include "Utilities/Map.hpp"
#include "ai.h"
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
        aiProcess();
    }
}
