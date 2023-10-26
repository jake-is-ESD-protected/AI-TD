#include "SmootherExponential.h"

SmootherExponentialSingleStage::SmootherExponentialSingleStage()
    : sample_rate(0.0),
      state(0.0),
      alpha_attack(0.0),
      alpha_release(0.0)
{
}

void SmootherExponentialSingleStage::reset(double _sample_rate)
{
    sample_rate = _sample_rate;
    state = 0.0;
}

void SmootherExponentialSingleStage::set_attack(double time_ms, int num_stages)
{
    // Convert time to seconds
    const double time_s = time_ms / 1000.0;

    // Correct time for cascaded stages
    const double cm = cutoff_multiplier(num_stages);

    alpha_attack = exp(-bdsp::constants::TWO_PI * cm / (sample_rate * time_s));
}

void SmootherExponentialSingleStage::set_release(double time_ms, int num_stages)
{
    // Convert time to seconds
    const double time_s = time_ms / 1000.0;

    const double cm = cutoff_multiplier(num_stages);
    alpha_release = exp(-bdsp::constants::TWO_PI * cm / (sample_rate * time_s));
}

double SmootherExponentialSingleStage::process(double x)
{
    // Find output

    double y = 0.0;
    double alpha = 0.0;
    if (x > state)
    {
        alpha = alpha_attack;
    }
    else
    {
        alpha = alpha_release;
    }

    y = x + alpha * (state - x);

    // Update state
    state = y;

    return y;
}

double SmootherExponentialSingleStage::cutoff_multiplier(int num_stages)
{
    // Helper function to be used in correction of time constants when cascading stages

    const double cm = 1.0 / (sqrt(pow(2.0, 1.0 / num_stages) - 1.0));
    return cm;
}

SmootherExponential::SmootherExponential() : num_stages(4){};

void SmootherExponential::reset(double sample_rate)
{
    for (int n = 0; n < num_stages; ++n)
    {
        smoothers[n].reset(sample_rate);
    }
}

void SmootherExponential::set_attack(double time_ms, int num_stages)
{
    for (int n = 0; n < num_stages; ++n)
    {
        smoothers[n].set_attack(time_ms, num_stages);
    }
}

void SmootherExponential::set_release(double time_ms, int num_stages)
{
    for (int n = 0; n < num_stages; ++n)
    {
        smoothers[n].set_release(time_ms, num_stages);
    }
}

double SmootherExponential::process(double x)
{
    for (int n = 0; n < num_stages; ++n)
    {
        x = smoothers[n].process(x);
    }

    return x;
}