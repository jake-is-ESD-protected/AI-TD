#pragma once
#include "../../libK/Utilities/Knob.hpp"

using namespace k;

extern Knob KnobAttack;
extern Knob KnobAttackTime;
extern Knob KnobSustain;
extern Knob KnobSustainTime;

void UIinit();

double UIgetBipolarAttackValue();

double UIgetBipolarSustainValue();
