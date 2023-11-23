#include "mem.hpp"
#include "cli.hpp"

static DaisySeed *pHw = NULL;

static char DSY_SDRAM_BSS sdramBuf[SDRAM_BUF_SIZE];

void memInit(DaisySeed *_pHw)
{
    pHw = _pHw;
}

uint8_t memSdramWrite(void *buf, uint32_t len)
{
    static uint32_t i = 0;
    if (isEOF(buf))
    {
        i = 0;
        return MEM_FINISH;
    }
    char *dataPos = &((char *)buf)[BLOCK_PREFIX_LEN];
    memcpy((void *)&(sdramBuf[i * BLOCKSIZE_DATA]), dataPos, BLOCKSIZE_DATA);
    i++;
    if ((i * BLOCKSIZE_DATA) >= SDRAM_BUF_SIZE)
    {
        // buffer is full
    }
    // check if data is valid
    return MEM_BLOCK_OK;
}

uint8_t memQspiWrite(void *buf, uint32_t len)
{
    // check if data is valid or EOF
    return MEM_BLOCK_OK;
}

bool isEOF(void *buf)
{
    char blockIndicator[BLOCK_PREFIX_LEN + 1] = {0};
    strncpy(blockIndicator, (char *)buf, BLOCK_PREFIX_LEN);
    if (!strcmp(blockIndicator, EOF_FLAG))
    {
        return true;
    }
    return false;
}
