#include "transientDSP.hpp"
#include "../../libK/Utilities/Map.hpp"
#include "../../src/ui/ui.hpp"
#include "hal.hpp"
#include <math.h>

#define ATTACK_FILTER_DEFAULT_RELEASE_TIME 20
#define ATTACK_FILTER_FAST_ATTACK_TIME 0.1
#define SUSTAIN_FILTER_DEFAULT_ATTACK_TIME 100
#define SUSTAIN_FILTER_SLOW_RELEASE_TIME 800
#define BASE_GAIN 0.5

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
    const double sFFilterV = sFFilter.process(env);
    const double sSFilterV = sSFilter.process(env);
    const double outputGain = BASE_GAIN + (getBipolarAttackValue() * fabs(aFFilterV - aSFilterV)) + (getBipolarSustainValue() * fabs(sSFilterV - sFFilterV));
    write2VCA(outputGain);
}

void uiProcessTransientDSP()
{
    aSFilter.set_attack(Map::mapClip(KnobAttackTime.getValue(), 0, 1, 0.1, 400));
    aSFilter.set_release(ATTACK_FILTER_DEFAULT_RELEASE_TIME);
    aFFilter.set_release(ATTACK_FILTER_DEFAULT_RELEASE_TIME);

    sSFilter.set_attack(SUSTAIN_FILTER_DEFAULT_ATTACK_TIME);
    sFFilter.set_attack(SUSTAIN_FILTER_DEFAULT_ATTACK_TIME);
    sFFilter.set_release(Map::mapClip(KnobSustainTime.getValue(), 1, 0, 0.1, 500));
}

void initTransientDSP()
{
    envFollower.init(sampleRate, 1);

    aFFilter.init(4);
    aFFilter.reset(sampleRate);
    aFFilter.set_attack(ATTACK_FILTER_FAST_ATTACK_TIME);
    aFFilter.set_release(ATTACK_FILTER_DEFAULT_RELEASE_TIME);

    aSFilter.init(2);
    aSFilter.reset(sampleRate);
    aSFilter.set_attack(100);
    aSFilter.set_release(ATTACK_FILTER_DEFAULT_RELEASE_TIME);

    sFFilter.init(2);
    sFFilter.reset(sampleRate);
    sFFilter.set_attack(SUSTAIN_FILTER_DEFAULT_ATTACK_TIME);
    sFFilter.set_release(50);

    sSFilter.init(4);
    sSFilter.reset(sampleRate);
    sSFilter.set_attack(SUSTAIN_FILTER_DEFAULT_ATTACK_TIME);
    sSFilter.set_release(SUSTAIN_FILTER_SLOW_RELEASE_TIME);
}