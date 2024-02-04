#include "ui.hpp"
#include "hal.hpp"

#include "../../libK/Utilities/Knob.hpp"
#include "../../libK/Utilities/Map.hpp"

using namespace k;

Knob KnobAttack = Knob(0.1, 1.0);
Knob KnobAttackTime = Knob(0.1, 1.0);
Knob KnobSustain = Knob(0.1, 0.7);
Knob KnobSustainTime = Knob(0.1, 1.0);

#define ATTACK_BOOST_GAIN 0.6
#define RELEASE_BOOST_GAIN 0.8

#define ATTACK_ATT_GAIN 0.6
#define RELEASE_ATT_GAIN 0.8

void UIinit()
{
}

double UIgetBipolarAttackValue()
{
    const double window = 0.1;
    if (KnobAttack.getValue() > 0.5 + window)
    {
        return ATTACK_BOOST_GAIN * 2.0 * (KnobAttack.getValue() - 0.5); // PLUS
    }
    else if (KnobAttack.getValue() < 0.5 - window)
    {
        return ATTACK_ATT_GAIN * 2.0 * (KnobAttack.getValue() - 0.5); // MINUS
    }
    else
        return 0;
}

double UIgetBipolarSustainValue()
{
    const double window = 0.1;
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