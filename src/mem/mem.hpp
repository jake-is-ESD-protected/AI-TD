#pragma once

#include "daisy_seed.h"

#define MEM_BLOCK_ERR 0
#define MEM_BLOCK_OK 1
#define MEM_FINISH 2

#define BLOCKSIZE 1024

using namespace daisy;

void memInit(DaisySeed *_pHw);

uint8_t memSdramWrite(void *buf, uint32_t len);

uint8_t memQspiWrite(void *buf, uint32_t len);