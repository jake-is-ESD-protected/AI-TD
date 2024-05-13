#pragma once

#include "./envDeps/BufferCircularAF.h"

void EnvelopeFollowerPeakHoldInitAf(double window_size_ms);

double EnvelopeFollowerPeakHoldProcessAf(double x);