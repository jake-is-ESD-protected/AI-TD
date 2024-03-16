#include "EnvelopeFollowerPeakHold.h"
#include "./envDeps/BufferCircular.h"
#include "../../hal/globalDefinitions.hpp"
#include <inttypes.h>

void EnvelopeFollowerPeakHoldInit(double window_size_ms)
{
    // prev_max = 0.0;
    BufferCircularInit((unsigned int)(window_size_ms / 1000.0 * sampleRate));
}

uint16_t blockCounter = 0;
uint16_t blockThreshold = 8;
double heldMax = 0;

double EnvelopeFollowerPeakHoldProcess(double x)
{
    BufferCircularPush_sample(x);
    if (blockCounter > blockThreshold)
    {
        blockCounter = 0;
        heldMax = BufferCircularMax();
    }

    blockCounter++;

    return heldMax;
}