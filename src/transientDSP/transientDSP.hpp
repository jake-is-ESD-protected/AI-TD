#pragma once
#include "EnvelopeFollowerPeakHold.h"
extern "C"
{
#include "SmootherExponential.h"
}

extern double lastVarGainValue;

extern SmootherExponential attackFastF;
extern SmootherExponential attackSlowF;

extern SmootherExponential sustainFastF;
extern SmootherExponential sustainSlowF;

void transientDSPinit();

void transientDSPprocess(double in);

void transientDSPuiProcess();
