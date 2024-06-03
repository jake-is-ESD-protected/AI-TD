#pragma once

#include "../af.h"

extern __attribute__((section(".sdram_bss"))) double magnitudeBeatBuffer[MAX_ONSETS][FFT_N2_LENGTH];

double calculateSpectralCentroid(uint64_t onsetIndex);

double calculateBandL(uint64_t onsetIndex);

double calculateBandML(uint64_t onsetIndex);

double calculateBandMH(uint64_t onsetIndex);

double calculateBandH(uint64_t onsetIndex);

double calculateCrestFactor(double* buffer, uint64_t audioIndex, uint64_t length);