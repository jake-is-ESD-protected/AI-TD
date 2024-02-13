#include "ui.hpp"
#include "hal.hpp"

#include "../../libK/Utilities/Knob.hpp"
#include "../../libK/Utilities/Map.hpp"

using namespace k;

Knob KnobAttack = Knob(0.1, 0.5);
Knob KnobAttackTime = Knob(0.1, 0.5);
Knob KnobSustain = Knob(0.1, 0.5);
Knob KnobSustainTime = Knob(0.1, 0.5);

#define ATTACK_BOOST_GAIN 0.6
#define RELEASE_BOOST_GAIN 0.7

#define ATTACK_ATT_GAIN 0.6
#define RELEASE_ATT_GAIN 0.8

void UIinit()
{
}

#define ATTACK_BOOST_SKEW 1.2
#define ATTACK_ATT_SKEW 1.6

double UIgetBipolarAttackValue()
{
    const double window = 0.05;
    if (KnobAttack.getValue() > 0.5 + window)
    {
        return ATTACK_BOOST_GAIN * Map::mapSkew(2.0 * (KnobAttack.getValue() - 0.5), ATTACK_BOOST_SKEW); // PLUS
    }
    else if (KnobAttack.getValue() < 0.5 - window)
    {
        return ATTACK_ATT_GAIN * 2.0 * ((Map::mapSkew(KnobAttack.getValue() * 2, ATTACK_ATT_SKEW) / 2.0) - 0.5); // MINUS
    }
    else
        return 0;
}

double UIgetBipolarSustainValue()
{
    const double window = 0.05;
    if (KnobSustain.getValue() > 0.5 + window)
    {
        return RELEASE_BOOST_GAIN * 2.0 * (KnobSustain.getValue() - 0.5); // PLUS
    }
    else if (KnobSustain.getValue() < 0.5 - window)
    {
        return RELEASE_ATT_GAIN * 2.0 * (KnobSustain.getValue() - 0.5); // MINUS
    }
    else
        return 0;
}