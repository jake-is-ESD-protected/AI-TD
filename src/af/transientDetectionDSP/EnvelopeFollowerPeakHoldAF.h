#pragma once

#include "./envDeps/BufferCircularAF.h"

void EnvelopeFollowerPeakHoldInitAf(float window_size_ms);

float EnvelopeFollowerPeakHoldProcessAf(float x);