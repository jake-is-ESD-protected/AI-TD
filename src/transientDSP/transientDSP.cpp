#include "transientDSP.hpp"
#include "../../libK/Utilities/Map.hpp"
#include "../../src/ui/ui.hpp"
#include "hal.hpp"
#include <math.h>

#define ATTACK_FILTER_DEFAULT_RELEASE_TIME 450 // TO LOW VALUES CAN LEAD TO ISSUES IN TALE OF NOTE //THE MORE RELEASE THE SOUND THE LONGER THIS NEEDS TO BE
#define ATTACK_FILTER_FAST_ATTACK_TIME 1
#define SUSTAIN_FILTER_DEFAULT_ATTACK_TIME 350
#define SUSTAIN_FILTER_FAST_RELEASE_TIME 100
#define SUSTAIN_FILTER_SLOW_RELEASE_TIME 600

static EnvelopeFollowerPeakHold envFollower;
double baseGain = 0.87;
double inputVolume = 1;
double lastVarGainValue = 0;

SmootherExponential attackFastF;
SmootherExponential attackSlowF;

SmootherExponential sustainFastF;
SmootherExponential sustainSlowF;

void transientDSPinit()
{
    envFollower.init(sampleRate, 1);

    attackFastF.init(4);
    attackFastF.reset(sampleRate);
    attackFastF.set_attack(ATTACK_FILTER_FAST_ATTACK_TIME);
    attackFastF.set_release(ATTACK_FILTER_DEFAULT_RELEASE_TIME);

    attackSlowF.init(4);
    attackSlowF.reset(sampleRate);
    attackSlowF.set_attack(100);
    attackSlowF.set_release(ATTACK_FILTER_DEFAULT_RELEASE_TIME);

    sustainFastF.init(4);
    sustainFastF.reset(sampleRate);
    sustainFastF.set_attack(900);
    sustainFastF.set_release(SUSTAIN_FILTER_FAST_RELEASE_TIME);

    sustainSlowF.init(4);
    sustainSlowF.reset(sampleRate);
    sustainSlowF.set_attack(450);
    sustainSlowF.set_release(SUSTAIN_FILTER_SLOW_RELEASE_TIME);
}

void transientDSPprocess(double in)
{
    if (halButtonRead())
    {
        const double env = envFollower.process(fabs(in * inputVolume));
        const double attackFastFV = attackFastF.process(env);
        const double attackSlowFV = attackSlowF.process(env);
        const double sustainFastFV = sustainFastF.process(env);
        const double sustainSlowFV = sustainSlowF.process(env);
        lastVarGainValue = (UIgetBipolarAttackValue() * fabs(attackFastFV - attackSlowFV)) + (UIgetBipolarSustainValue() * fabs(sustainFastFV - sustainSlowFV));

        halVCAwrite(baseGain + lastVarGainValue);
    }
    else
    {
        halVCAwrite(baseGain);
    }
}

void transientDSPuiProcess()
{
    inputVolume = Map::mapClip(KnobAttack.getShiftValue(), 0, 1, 0.1, 1.5);
    baseGain = Map::mapClip(Map::mapSkew(KnobSustain.getShiftValue(), 2.8), 0, 1, 0.1, 0.87);
    attackSlowF.set_attack(Map::mapClip(Map::mapSkew(KnobAttackTime.getValue(), 0.6), 0, 1, 14, 500));
    attackSlowF.set_release(Map::mapClip(KnobAttackTime.getShiftValue(), 1, 0, 200, 900));
    sustainSlowF.set_release(Map::mapClip(Map::mapSkew(KnobSustainTime.getValue(), 0.5), 0, 1, 150, 3000));
    sustainFastF.set_release(Map::mapClip(KnobSustainTime.getShiftValue(), 0, 1, SUSTAIN_FILTER_FAST_RELEASE_TIME, 600));
}
