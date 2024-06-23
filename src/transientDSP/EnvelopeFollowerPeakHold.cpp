#include "EnvelopeFollowerPeakHold.h"

EnvelopeFollowerPeakHold::EnvelopeFollowerPeakHold() : sample_rate(0.0)
{
}

void EnvelopeFollowerPeakHold::init(float _sample_rate, float window_size_ms)
{
    sample_rate = _sample_rate;
    // prev_max = 0.0;
    state.init((unsigned int)(window_size_ms / 1000.0 * sample_rate));
}

uint16_t blockCounter = 0;
uint16_t blockThreshold = 8;
float heldMax = 0;

float EnvelopeFollowerPeakHold::process(float x)
{
    state.push_sample(x);
    if (blockCounter > blockThreshold)
    {
        blockCounter = 0;
        heldMax = state.max();
    }

    blockCounter++;

    return heldMax;
}