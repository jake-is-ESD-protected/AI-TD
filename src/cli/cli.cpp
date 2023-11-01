#include "cli.hpp"

static uint8_t rxBuf[CLI_RX_BUF_SIZE] = {0};
static DaisySeed *pHw = NULL;

void cliInit(DaisySeed *_pHw)
{
    pHw = _pHw;
    pHw->usb_handle.Init(UsbHandle::FS_INTERNAL);
    pHw->usb_handle.SetReceiveCallback(cliRXCallback, UsbHandle::FS_INTERNAL);
}

void cliRXCallback(uint8_t *buf, uint32_t *len)
{
    uint32_t i = 0;
    while (i < *len && i < CLI_RX_BUF_SIZE)
    {
        rxBuf[i] = buf[i];
        i++;
    }
}

void cliPrintBuf(uint8_t *buf, uint32_t len)
{
    pHw->usb_handle.TransmitInternal((uint8_t *)buf, len);
}

void cliPrintStr(const char *str)
{
    cliPrintBuf((uint8_t *)str, strlen(str));
}

void cliParse(void *cmd, uint32_t len)
{
    /// TODO:
}

void cliPipe(void *src_buf, consumer process)
{
    /// TODO:
}