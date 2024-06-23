#pragma once
#include "Utilities/Knob.hpp"

using namespace k;

extern Knob KnobAttack;
extern Knob KnobAttackTime;
extern Knob KnobSustain;
extern Knob KnobSustainTime;

void UIinit();

float UIgetBipolarAttackValue();

float UIgetBipolarSustainValue();
