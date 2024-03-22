#include "SmootherExponential.h"
#include "../hal/globalDefinitions.hpp"

SmootherExponentialSingleStage::SmootherExponentialSingleStage() : sample_rate(0.0), state(0.0), alpha_attack(0.0), alpha_release(0.0)
{
}

void SmootherExponentialSingleStage::reset(double _sample_rate)
{
    sample_rate = _sample_rate;
    state = 0.0;
}

void SmootherExponentialSingleStage::set_attack(double time_ms, double cm)
{
    // Convert time to seconds
    const double time_s = time_ms / 1000.0;
    alpha_attack = exp(-TWO_PI * cm / (sample_rate * time_s));
}

void SmootherExponentialSingleStage::set_release(double time_ms, double cm)
{
    // Convert time to seconds
    const double time_s = time_ms / 1000.0;
    alpha_release = exp(-TWO_PI * cm / (sample_rate * time_s));
}

double SmootherExponentialSingleStage::process(double x)
{
    // Find output
    double alpha = (x > state) ? alpha_attack : alpha_release;
    double y = alpha * (state - x) + x;

    // Update state
    state = y;

    return y;
}

SmootherExponential::SmootherExponential() : num_stages(0){};

void SmootherExponential::init(int _num_stages)
{
    num_stages = _num_stages;
    cm = 1.0 / (sqrt(pow(2.0, 1.0 / num_stages) - 1.0));
    smoothers.reset(new SmootherExponentialSingleStage[num_stages]);
}

void SmootherExponential::reset(double sample_rate)
{
    for (int n = 0; n < num_stages; ++n)
    {
        smoothers[n].reset(sample_rate);
    }
}

void SmootherExponential::set_attack(double time_ms, int num_stages)
{
    attack_time = time_ms;
    for (int n = 0; n < num_stages; ++n)
    {
        smoothers[n].set_attack(time_ms, cm);
    }
}

void SmootherExponential::set_release(double time_ms, int num_stages)
{
    release_time = time_ms;
    for (int n = 0; n < num_stages; ++n)
    {
        smoothers[n].set_release(time_ms, cm);
    }
}

double SmootherExponential::get_attack()
{
    return attack_time;
}

double SmootherExponential::get_release()
{
    return release_time;
}

double SmootherExponential::process(double x)
{
    for (int n = 0; n < num_stages; ++n)
    {
        x = smoothers[n].process(x);
    }
    return x;
}