#pragma once

#include "daisy_seed.h"

#define sampleRate 96000
#define LED_DISPLAY_GAIN 3.5

using namespace daisy;

/// @brief
void halInit();

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

/// @brief
/// @param cb
void halUsbCdcInit(void (*cb)(uint8_t *buf, uint32_t *len));

/// @brief
/// @param buf
/// @param len
void halUsbCdcTransmit(uint8_t *buf, uint32_t len);

/// @brief
/// @param mem
/// @param len
void halEraseQspiFlash(uint8_t *mem, uint32_t len);

/// @brief
/// @param start
/// @param len
/// @param content
void halWriteQspiFlash(uint32_t start, uint32_t len, uint8_t *content);