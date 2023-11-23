#include "mem.hpp"
#include "cli.hpp"

static DaisySeed *pHw = NULL;

static char DSY_SDRAM_BSS sdramBuf[SDRAM_BUF_SIZE];

void memInit(DaisySeed *_pHw)
{
    pHw = _pHw;
}

uint8_t memSdramWrite(void *buf, uint32_t len, void *args)
{
    static uint32_t i = 0;
    if (isEOF(buf))
    {
        uint16_t crc = crc16(&((const uint8_t *)sdramBuf)[0], i * BLOCKSIZE_DATA);
        memcpy((void *)&(sdramBuf[i * BLOCKSIZE_DATA]), (void *)&crc, sizeof(crc));
        *(uint16_t *)args = crc;
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

uint8_t memQspiWrite(void *buf, uint32_t len, void *args)
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

uint16_t crc16(const uint8_t *data, uint32_t len)
{
    uint16_t crc = CRC_INITIAL_VALUE;
    uint32_t i, j;
    for (i = 0; i < len; ++i)
    {
        crc ^= (uint16_t)data[i];
        for (j = 0; j < 8; ++j)
        {
            if (crc & 1)
            {
                crc = (crc >> 1) ^ CRC_POLYNOMIAL;
            }
            else
            {
                crc >>= 1;
            }
        }
    }
    return crc;
}