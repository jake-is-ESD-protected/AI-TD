#include "mem.hpp"

static DaisySeed *pHw = NULL;

void memInit(DaisySeed *_pHw)
{
    pHw = _pHw;
}

uint8_t memSdramWrite(void *buf, uint32_t len)
{
    static uint32_t i = 0;
    // check if data is valid or EOF
    return MEM_BLOCK_OK;
}

uint8_t memQspiWrite(void *buf, uint32_t len)
{
    // check if data is valid or EOF
    return MEM_BLOCK_OK;
}