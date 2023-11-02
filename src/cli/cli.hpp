#pragma once

#include "daisy_seed.h"

using namespace daisy;

#define CLI_RX_BUF_SIZE 1024

#define CMD_GET_STAT "stat"
#define CMD_START_TRANSFER "send"
#define CMD_STOP_TRANSFER "stop"

typedef enum cmd
{
    CMD_get_stat,
    CMD_start_transfer,
    CMD_stop_transfer,
    CMD_num_cmds
} cmd_t;

typedef enum cli_state
{
    CLI_STATE_closed,
    CLI_STATE_idle,
    CLI_STATE_stream,
    CLI_STATE_num_states
} cli_state_t;

typedef void (*consumer)(void *, uint32_t);

/// @brief Initialize CLI. Sets physical micro-USB port as USB-CDC device
/// which can be addressed as normal COM port or ttyACM device for serial
/// communication.
/// @param phw pointer to singleton Daisy Seed hardware instance
/// @note Without further config, the default baud rate is 115200
void cliInit(DaisySeed *_pHw);

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

/// @brief
/// @param err
/// @param len
void cliErrHandler(void *err, uint32_t len);