#include "../../hal/globalDefinitions.hpp"
#include <math.h>
#include "inttypes.h"
#include "Smoother4Exponential.h"

smootherType smoothers[4];

void reset(smootherType *smoother)
{
    smoother->state = 0.0;
}

void setAttack(smootherType *smoother, double time_ms)
{
    // Convert time to seconds
    const double time_s = time_ms / 1000.0;
    smoother->alpha_attack = exp(-TWO_PI * CM / (sampleRate * time_s));
}

void setRelease(smootherType *smoother, double time_ms)
{
    // Convert time to seconds
    const double time_s = time_ms / 1000.0;

    smoother->alpha_release = exp(-TWO_PI * CM / (  sampleRate * time_s));
}

double process(smootherType *smoother, double x)
{
    // Find output

    double y = 0.0;
    double alpha = 0.0;
    if (x > smoother->state)
    {
        alpha = smoother->alpha_attack;
    }
    else
    {
        alpha = smoother->alpha_release;
    }

    y = x + alpha * (smoother->state - x);

    // Update state
    smoother->state = y;

    return y;
}

void initAll4()
{
    resetAll4();
}

void resetAll4()
{
    for (int n = 0; n < 4; ++n)
    {
        reset(&smoothers[n]);
    }
}

void setAttackAll4(double time_ms)
{
    for (int n = 0; n < 4; ++n)
    {
        setAttack(&smoothers[n], time_ms);
    }
}

void setReleaseAll4(double time_ms)
{
    for (int n = 0; n < 4; ++n)
    {
        setRelease(&smoothers[n], time_ms);
    }
}

double processAll4(double x)
{
    for (int n = 0; n < 4; ++n)
    {
        x = process(&smoothers[n], x);
    }

    return x;
}