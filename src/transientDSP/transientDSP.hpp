#pragma once
#include "EnvelopeFollowerPeakHold.h"
extern "C"
{
#include "SmootherExponential.h"
}

extern float aiAttack;
extern float aiSustain;

extern float lastVarGainValue;

extern SmootherExponential attackFastF;
extern SmootherExponential attackSlowF;

extern SmootherExponential sustainFastF;
extern SmootherExponential sustainSlowF;

void transientDSPinit();

void transientDSPprocess(float in);

void transientDSPuiProcess();
