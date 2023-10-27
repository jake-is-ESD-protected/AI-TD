#include "transientDSP.hpp"
#include "../../libK/Utilities/Map.hpp"
#include "../../src/ui/ui.hpp"
#include "hal.hpp"
#include <math.h>

static EnvelopeFollowerPeakHold envFollower;

SmootherExponential aFFilter;
SmootherExponential aSFilter;

SmootherExponential sFFilter;
SmootherExponential sSFilter;

void processTransientDSP(double in)
{
    const double env = envFollower.process(fabs(in));
    const double aFFilterV = aFFilter.process(env);
    const double aSFilterV = aSFilter.process(env);
    write2VCA(fabs(aFFilterV - aSFilterV));
}

void uiProcessTransientDSP()
{
    aSFilter.set_attack(Map::mapClip(KnobAttackTime.getValue(), 0, 1, 0.1, 300));
    aSFilter.set_release(Map::mapClip(KnobSustainTime.getValue(), 0, 1, 1, 500));
    aFFilter.set_release(Map::mapClip(KnobSustainTime.getValue(), 0, 1, 1, 500));
}

void initTransientDSP()
{
    envFollower.init(96000, 1);

    aFFilter.init(4);
    aFFilter.reset(96000);
    aFFilter.set_attack(0.1);
    aFFilter.set_release(50);

    aSFilter.init(4);
    aSFilter.reset(96000);
    aSFilter.set_attack(100);
    aSFilter.set_release(50);
}