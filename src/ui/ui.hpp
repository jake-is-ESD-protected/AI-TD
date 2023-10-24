#ifndef UI_H
#define UI_H
#include "../../libK/Utilities/Knob.hpp"

using namespace k;

extern Knob KnobAttack;
extern Knob KnobAttackTime;
extern Knob KnobSustain;
extern Knob KnobSustainTime;

void initUI();

void processUI();

#endif