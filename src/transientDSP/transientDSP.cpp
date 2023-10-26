#include "transientDSP.hpp"
#include <math.h>
#include "hal.hpp"
#include "SmootherExponential.h"
#include "EnvelopeFollowerPeakHold.h"

static SmootherExponential envSmoother;
static EnvelopeFollowerPeakHold envFollower;

void processTransientDSP(double in)
{
	const double env = envFollower.process(fabs(in));
	const double envSmooth = envSmoother.process(env);
	write2VCA(envSmooth);
}

void initTransientDSP()
{
	envSmoother.reset(96000);
	envSmoother.set_attack(1);
	envSmoother.set_release(100);
	envFollower.init(96000, 2);
}