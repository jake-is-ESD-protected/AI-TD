#include "transientDSP.hpp"
#include <math.h>
#include "hal.hpp"

static EnvelopeFollowerPeakHold envFollower;
SmootherExponential envSmoother;

void processTransientDSP(double in)
{
	const double env = envFollower.process(fabs(in));
	const double envSmooth = envSmoother.process(env);
	write2VCA(envSmooth);
}

void initTransientDSP()
{
	envFollower.init(96000, 1);
	envSmoother.init(4);
	envSmoother.reset(96000);
	envSmoother.set_attack(1);
	envSmoother.set_release(50);
}