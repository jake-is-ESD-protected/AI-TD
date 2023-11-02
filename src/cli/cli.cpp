#include "cli.hpp"
#include "hal.hpp"

static uint8_t DSY_SDRAM_DATA rxBuf[CLI_RX_BUF_SIZE] = {0};
static DaisySeed *pHw = NULL;
static cli_state_t stat = CLI_STATE_idle;

void cliInit(DaisySeed *_pHw)
{
    pHw = _pHw;
    pHw->usb_handle.Init(UsbHandle::FS_INTERNAL);
    System::Delay(1000);
    pHw->usb_handle.SetReceiveCallback(cliRXCallback, UsbHandle::FS_INTERNAL);
    System::Delay(1000);
}

void cliRXCallback(uint8_t *buf, uint32_t *len)
{
    // halStopAudio();
    static consumer consFunction = NULL;
    uint32_t i = 0;

    while (i < *len && i < CLI_RX_BUF_SIZE)
    {
        rxBuf[i] = buf[i];
        i++;
    }

    switch (stat)
    {
    case CLI_STATE_idle:
        consFunction = cliParse;
        break;
    case CLI_STATE_stream:
        consFunction = NULL; // add a send to qspi-func here
        break;
    default:
        consFunction = cliErrHandler;
        break;
    }
    consFunction((void *)rxBuf, *len);
    // halStartAudio();
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
    char *msg = (char *)cmd;
    char *mainCmd = strtok(msg, " ");

    if (!strcmp(mainCmd, CMD_GET_STAT))
    {
        /// TODO:
        cliPrintStr("Not yet implemented.\n\r");
    }
    else if (!strcmp(mainCmd, CMD_START_TRANSFER))
    {
        cliPrintStr("Awaiting data transfer...\n\r");
        stat = CLI_STATE_stream;
    }
    else if (!strcmp(mainCmd, CMD_STOP_TRANSFER))
    {
        cliPrintStr("Data transfer completed.\n\r");
        stat = CLI_STATE_idle;
    }
    else
    {
        /// TODO: Something went wrong
        cliPrintStr("Parse error: unknown command.\n\r");
    }
}

void cliErrHandler(void *err, uint32_t len)
{
}