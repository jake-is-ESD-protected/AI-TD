#ifndef _CLI_HPP_
#define _CLI_HPP_

#include "daisy_seed.h"

using namespace daisy;

#define CLI_RX_BUF_SIZE 1024

/// @brief Initialize CLI. Sets physical micro-USB port as USB-CDC device
/// which can be addressed as normal COM port or ttyACM device for serial
/// communication.
/// @param phw pointer to singleton Daisy Seed hardware instance
/// @note Without further config, the default baud rate is 115200
void cliInit(DaisySeed *_phw);

/// @brief Callback function on receiving data via the virtual COM port.
/// @param buf Buffer filled by HAL-internal call containing the received bytes.
/// @param len Length of data in buffer in bytes.
void cliRXCallback(uint8_t *buf, uint32_t *len);

/// @brief Print the contents of a buffer via the virtual COM port.
/// @param buf Buffer to print. Can be any data, will be interpreted as `char`.
/// @param len Length of data in buffer in bytes.
void cliPrintBuf(uint8_t *buf, uint32_t len);

/// @brief Print a string via the virtual COM port.
/// @param str String to print. EOL is determined by the `\0` symbol. Use
/// `cliPrintBuf` to print raw data without the `\0` terminator.
void cliPrintStr(const char *str);

#endif