#include "cli.hpp"

static uint8_t rxBuf[CLI_RX_BUF_SIZE] = {0};
static DaisySeed *phw = NULL;

void cliInit(DaisySeed *_phw)
{
    phw = _phw;
    phw->usb_handle.Init(UsbHandle::FS_INTERNAL);
    phw->usb_handle.SetReceiveCallback(cliRXCallback, UsbHandle::FS_INTERNAL);
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
    phw->usb_handle.TransmitInternal((uint8_t *)buf, len);
}

void cliPrintStr(const char *str)
{
    cliPrintBuf((uint8_t *)str, strlen(str));
}