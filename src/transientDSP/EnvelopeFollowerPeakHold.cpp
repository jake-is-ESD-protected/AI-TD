#include "EnvelopeFollowerPeakHold.h"

EnvelopeFollowerPeakHold::EnvelopeFollowerPeakHold()
    : sample_rate(0.0)
{
}

void EnvelopeFollowerPeakHold::init(double _sample_rate, double window_size_ms)
{
    sample_rate = _sample_rate;
    //prev_max = 0.0;
    state.init((unsigned int)(window_size_ms / 1000.0 * sample_rate));
}

double EnvelopeFollowerPeakHold::process(double x)
{
    state.push_sample(x);
    const double max = state.max(); //THIS NEEDS BLOCK PROCESSING

    return max;
}