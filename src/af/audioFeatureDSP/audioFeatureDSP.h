#pragma once

#include "../af.h"

extern __attribute__((section(".sdram_bss"))) float magnitudeBeatBuffer[MAX_ONSETS][FFT_N2_LENGTH];

float calculateSpectralCentroid(uint64_t onsetIndex);

float calculateBandL(uint64_t onsetIndex);

float calculateBandML(uint64_t onsetIndex);

float calculateBandMH(uint64_t onsetIndex);

float calculateBandH(uint64_t onsetIndex);

float calculateCrestFactor(float* buffer, uint64_t audioIndex, uint64_t length);