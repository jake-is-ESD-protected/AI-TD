#pragma once

#include "daisy_seed.h"

#define sampleRate 96000
#define LED_DISPLAY_GAIN 3.5

using namespace daisy;

/// @brief
/// @param _pHw
void halInit(DaisySeed *_pHw);

/// @brief
/// @param value
void halVCAwrite(double value);

/// @brief
void halTimerInit();

/// @brief
/// @param b
void halLEDset(bool b);

/// @brief
/// @return
bool halButtonRead();
