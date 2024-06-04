#pragma once

#include "daisy_seed.h"
#include "globalDefinitions.hpp"

#define LED_DISPLAY_GAIN 10

using namespace daisy;

extern bool processAFFlag;

/// @brief
void halInit();

void resetShiftLayer();

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

/// @brief
void halStartAudio();

/// @brief
void halStopAudio();

extern DaisySeed hw;