#include "ui.hpp"
#include "hal.hpp"

#include "../../libK/Utilities/Knob.hpp"
#include "../../libK/Utilities/Map.hpp"

using namespace k;

Knob KnobAttack = Knob(0.1);
Knob KnobAttackTime = Knob(0.1);
Knob KnobSustain = Knob(0.1);
Knob KnobSustainTime = Knob(0.1);

void UIinit()
{
}

double UIgetBipolarAttackValue()
{
    const double window = 0.1;
    if (KnobAttack.getValue() > 0.5 + window)
    {
        return 2.0 * (KnobAttack.getValue() - 0.5); // PLUS
    }
    else if (KnobAttack.getValue() < 0.5 - window)
    {
        return 2.0 * (KnobAttack.getValue() - 0.5); // MINUS
    }
    else
        return 0;
}

double UIgetBipolarSustainValue()
{
    const double window = 0.1;
    if (KnobSustain.getValue() > 0.5 + window)
    {
        return 2.0 * (KnobSustain.getValue() - 0.5); // PLUS
    }
    else if (KnobSustain.getValue() < 0.5 - window)
    {
        return 2.0 * (KnobSustain.getValue() - 0.5); // MINUS
    }
    else
        return 0;
}