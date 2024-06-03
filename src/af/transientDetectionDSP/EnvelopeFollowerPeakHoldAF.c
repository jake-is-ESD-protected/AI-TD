#include "EnvelopeFollowerPeakHoldAF.h"
#include "./envDeps/BufferCircularAF.h"
#include "../../hal/globalDefinitions.hpp"
#include <inttypes.h>

void EnvelopeFollowerPeakHoldInitAf(double window_size_ms)
{
    // prev_max = 0.0;
    BufferCircularInitAf((unsigned int)(window_size_ms / 1000.0 * sampleRate));
}

uint16_t blockCounterAf = 0;
uint16_t blockThresholdAf = 8;
double heldMaxAf = 0;

double EnvelopeFollowerPeakHoldProcessAf(double x)
{
    BufferCircularPush_sampleAf(x);
    if (blockCounterAf > blockThresholdAf)
    {
        blockCounterAf = 0;
        heldMaxAf = BufferCircularMaxAf();
    }

    blockCounterAf++;

    return heldMaxAf;
}