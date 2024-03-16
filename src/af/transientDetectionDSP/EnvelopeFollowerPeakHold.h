#pragma once

#include "./envDeps/BufferCircular.h"

void EnvelopeFollowerPeakHoldInit(double window_size_ms);

double EnvelopeFollowerPeakHoldProcess(double x);