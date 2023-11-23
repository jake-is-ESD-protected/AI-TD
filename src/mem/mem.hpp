// #pragma once
#ifndef _MEM_HPP_
#define _MEM_HPP_

#include "daisy_seed.h"

#define MEM_BLOCK_ERR 0
#define MEM_BLOCK_OK 1
#define MEM_FINISH 2

#define SDRAM_BUF_SIZE 65536
#define BLOCK_PREFIX_LEN 4
#define BLOCK_POSTFIX_LEN 4
#define BLOCKSIZE_RAW 64
#define BLOCKSIZE_DATA (BLOCKSIZE_RAW - BLOCK_PREFIX_LEN)

#define CRC_POLYNOMIAL 0x8408
#define CRC_INITIAL_VALUE 0xFFFF

using namespace daisy;

void memInit(DaisySeed *_pHw);

uint8_t memSdramWrite(void *buf, uint32_t len, void *args);

uint8_t memQspiWrite(void *buf, uint32_t len, void *args);

bool isEOF(void *buf);

uint16_t crc16(const uint8_t *data, uint32_t len);

#endif