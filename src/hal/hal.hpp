#ifndef HAL_H
#define HAL_H

#include "daisy_seed.h"

#define sampleRate 96000

void write2VCA(double value);

void initHal();

void setLed(bool b);

bool readButton();

void initTimer();

/// @brief Initialize CLI. Sets physical micro-USB port as USB-CDC device
/// which can be addressed as normal COM port or ttyACM device for serial
/// communication.
void cliInit();

/// @brief Callback function on receiving data via the virtual COM port.
/// @param buf Buffer filled by HAL-internal call containing the received bytes.
/// @param len Length of data in buffer in bytes.
void cliRXCallback(uint8_t* buf, uint32_t* len);

/// @brief Print the contents of a buffer via the virtual COM port.
/// @param buf Buffer to print. Can be any data, will be interpreted as `char`.
/// @param len Length of data in buffer in bytes.
void cliPrintBuf(uint8_t* buf, uint32_t len);

/// @brief Print a string via the virtual COM port.
/// @param str String to print. EOL is determined by the `\0` symbol. Use 
/// `cliPrintBuf` to print raw data without the `\0` terminator.
void cliPrintStr(const char* str);

#endif