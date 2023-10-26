#pragma once

#include "BufferCircular.h"

class EnvelopeFollowerPeakHold
{
public:
    EnvelopeFollowerPeakHold();
    ~EnvelopeFollowerPeakHold() = default;

    void init(double _sample_rate, double window_size_ms);

    //void set_decay(double decay_ms);

    double process(double x);

private:
    double sample_rate;

    bdsp::BufferCircular<double> state;

    //double prev_max;
    //double decrement;
};