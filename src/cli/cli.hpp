#ifndef _CLI_HPP_
#define _CLI_HPP_

#include "daisy_seed.h"

using namespace daisy;

#define CLI_RX_BUF_SIZE 1024

#define CMD_GET_STAT

typedef enum cmd
{
    CMD_unknown_cmd,
    CMD_get_stat,
    CMD_start_transfer,
    CMD_stop_transfer
} cmd_t;

typedef void (*consumer)(void *, uint32_t);

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

/// @brief Parse an incoming command from string format to a `cmd_t` enum.
/// @param cmd Incoming command in string format (buffer).
/// @param len Length of command in bytes.
void cliParse(void *cmd, uint32_t len);

/// @brief Send source data to a function which processes it.
/// @param src_buf Data source buffer.
/// @param process Function pointer to executing function. Has to
/// be of type `consumer` (*function)(void *, uint32_t).
void cliPipe(void *src_buf, consumer process);

#endif