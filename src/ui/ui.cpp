#include "ui.hpp"

#include "../../libK/Utilities/Knob.hpp"

using namespace k;

Knob KnobAttack = Knob(0.1);
Knob KnobAttackTime = Knob(0.1);
Knob KnobSustain = Knob(0.1);
Knob KnobSustainTime = Knob(0.1);

void initUI()
{
}

void processUI()
{
}

double getBipolarAttackValue()
{
    return 2.0 * (KnobAttack.getValue() - 0.5);
}

double getBipolarSustainValue()
{
    return 2.0 * (KnobSustain.getValue() - 0.5);
}