#include "transientDSP.hpp"
#include "../../libK/Utilities/Map.hpp"
#include "../../src/ui/ui.hpp"
#include "hal.hpp"
#include <math.h>

#define ATTACK_FILTER_DEFAULT_RELEASE_TIME 450 // TO LOW VALUES CAN LEAD TO ISSUES IN TALE OF NOTE //THE MORE RELEASE THE SOUND THE LONGER THIS NEEDS TO BE
#define ATTACK_FILTER_FAST_ATTACK_TIME 1
#define SUSTAIN_FILTER_DEFAULT_ATTACK_TIME 50
#define SUSTAIN_FILTER_FAST_RELEASE_TIME 100
#define SUSTAIN_FILTER_SLOW_RELEASE_TIME 600
#define BASE_GAIN 0.8
#define ATTACK_GAIN 0.9
#define RELEASE_GAIN 1.2

static EnvelopeFollowerPeakHold envFollower;

double lastVarGainValue = 0;

SmootherExponential aFFilter;
SmootherExponential aSFilter;

SmootherExponential sFFilter;
SmootherExponential sSFilter;

void transientDSPinit()
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
    sFFilter.set_release(SUSTAIN_FILTER_FAST_RELEASE_TIME);

    sSFilter.init(4);
    sSFilter.reset(sampleRate);
    sSFilter.set_attack(SUSTAIN_FILTER_DEFAULT_ATTACK_TIME);
    sSFilter.set_release(SUSTAIN_FILTER_SLOW_RELEASE_TIME);
}

void transientDSPprocess(double in)
{
    const double env = envFollower.process(fabs(in));
    const double aFFilterV = aFFilter.process(env);
    const double aSFilterV = aSFilter.process(env);
    const double sFFilterV = sFFilter.process(env);
    const double sSFilterV = sSFilter.process(env);
    lastVarGainValue = (UIgetBipolarAttackValue() * fabs(aFFilterV - aSFilterV) * ATTACK_GAIN) + (UIgetBipolarSustainValue() * fabs(sFFilterV - sSFilterV) * RELEASE_GAIN);
    if (halButtonRead())
        halVCAwrite(BASE_GAIN + lastVarGainValue);
    else
        halVCAwrite(BASE_GAIN);
}

void transientDSPuiProcess()
{
    aSFilter.set_attack(Map::mapClip(KnobAttackTime.getValue(), 0, 1, 0.1, 400));
    sSFilter.set_release(Map::mapClip(KnobSustainTime.getValue(), 1, 0, 0.1, 3000));
}
