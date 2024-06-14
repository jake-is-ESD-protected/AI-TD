#pragma once

#include "./envDeps/BufferCircular.h"

class EnvelopeFollowerPeakHold
{
  public:
    EnvelopeFollowerPeakHold();
    ~EnvelopeFollowerPeakHold() = default;

    void init(float _sample_rate, float window_size_ms);

    // void set_decay(float decay_ms);

    float process(float x);

  private:
    float sample_rate;

    bdsp::BufferCircular<float> state;

    // float prev_max;
    // float decrement;
};