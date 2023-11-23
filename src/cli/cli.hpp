#pragma once

#include "daisy_seed.h"
#include "mem.hpp"

using namespace daisy;

#define CLI_RX_BUF_SIZE BLOCKSIZE
#define CLI_TX_BUF_SIZE 64

#define CLI_STAT_ERR 10
#define CLI_STAT_OK 11

#define CLI_PREFIX "[SEED]:"
#define FLAGPREFIX "--"
#define LINE_ENDING "\n\r"

#define CMD_GET "get"
#define CMD_SET "set"
#define CMD_SEND "send"
#define CMD_DATA "data"

#define CMD_SEND_FLAG_SDRAM "--sdram"
#define CMD_SEND_FLAG_QSPI "--qspi"

#define RESPONSE_OK "OK"
#define RESPONSE_ERR "ERR"
#define RESPONSE_WARN "WARN"
#define RESPONSE_RDY "RDY"
#define RESPONSE_FNSH "FNSH"

#define PAD_BYTE '0'
#define EOF_FLAG "stop"

typedef enum cli_state
{
    CLI_STATE_closed,
    CLI_STATE_idle,
    CLI_STATE_stream_sdram,
    CLI_STATE_stream_qspi,
    CLI_STATE_num_states
} cli_state_t;

typedef uint8_t (*consumer)(void *, uint32_t, void *args);

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
/// @param type Designator string. Use the defines listed as "RESPONSE_X".
void cliPrintStr(const char *type, const char *str);

/// @brief Parse an incoming command from string format to a `cmd_t` enum.
/// @param cmd Incoming command in string format (buffer).
/// @param len Length of command in bytes.
/// @param args Optional arguments.
/// @return Status flag.
uint8_t cliParse(void *cmd, uint32_t len, void *args);

/// @brief
/// @param err
/// @param len
/// @param args Optional arguments.
/// @return Status flag.
uint8_t cliErrHandler(void *err, uint32_t len, void *args);