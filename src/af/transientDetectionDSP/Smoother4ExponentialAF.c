#include "../../hal/globalDefinitions.hpp"
#include <math.h>
#include "inttypes.h"
#include "Smoother4ExponentialAF.h"

smootherTypeAf smoothersAf[4];

void resetAf(smootherTypeAf *smoother)
{
    smoother->state = 0.0;
}

void setAttackAf(smootherTypeAf *smoother, double time_ms)
{
    // Convert time to seconds
    const double time_s = time_ms / 1000.0;
    smoother->alpha_attack = exp(-TWO_PI * CM / (slowsampleRate * time_s));
}

void setReleaseAf(smootherTypeAf *smoother, double time_ms)
{
    // Convert time to seconds
    const double time_s = time_ms / 1000.0;

    smoother->alpha_release = exp(-TWO_PI * CM / (  slowsampleRate * time_s));
}

double processAf(smootherTypeAf *smoother, double x)
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

void initAll4Af()
{
    resetAll4Af();
}

void resetAll4Af()
{
    for (int n = 0; n < 4; ++n)
    {
        resetAf(&smoothersAf[n]);
    }
}

void setAttackAll4Af(double time_ms)
{
    for (int n = 0; n < 4; ++n)
    {
        setAttackAf(&smoothersAf[n], time_ms);
    }
}

void setReleaseAll4Af(double time_ms)
{
    for (int n = 0; n < 4; ++n)
    {
        setReleaseAf(&smoothersAf[n], time_ms);
    }
}

double processEnvelopeAf(double x)
{
    //ADD PEAK HOLD SCHEME HERE
    
    for (int n = 0; n < 4; ++n)
    {
        x = processAf(&smoothersAf[n], x);
    }

    return x;
}