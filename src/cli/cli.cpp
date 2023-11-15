#include "cli.hpp"
#include "hal.hpp"
#include "mem/mem.hpp"

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
    case CLI_STATE_stream_sdram:
        consFunction = memSdramWrite;
        break;
    case CLI_STATE_stream_qspi:
        consFunction = memQspiWrite;
        break;
    default:
        consFunction = cliErrHandler;
        break;
    }
    uint8_t stat = consFunction((void *)rxBuf, *len);
    if (stat == MEM_FINISH)
    {
        stat = CLI_STATE_idle;
    }

    // halStartAudio();
}

void cliPrintBuf(uint8_t *buf, uint32_t len)
{
    pHw->usb_handle.TransmitInternal((uint8_t *)buf, len);
}

void cliPrintStr(const char *type, const char *str)
{
    char txBuf[CLI_TX_BUF_SIZE] = {0};
    sprintf(txBuf, "%s %s: %s", CLI_PREFIX, type, str);
    cliPrintBuf((uint8_t *)txBuf, strlen(txBuf));
}

uint8_t cliParse(void *cmd, uint32_t len)
{
    char *msg = (char *)cmd;
    char *mainCmd = strtok(msg, " ");

    if (!strcmp(mainCmd, CMD_GET))
    {
        cliPrintStr(RESPONSE_ERR, "Getter not yet implemented.\n\r");
        stat = CLI_STATE_idle;
    }
    else if (!strcmp(mainCmd, CMD_SET))
    {
        cliPrintStr(RESPONSE_ERR, "Setter not yet implemented.\n\r");
        stat = CLI_STATE_idle;
    }
    else if (!strcmp(mainCmd, CMD_SEND))
    {
        char *arg = strtok(NULL, " ");
        if (!strcmp(arg, CMD_SEND_FLAG_SDRAM))
        {
            stat = CLI_STATE_stream_sdram;
            cliPrintStr(RESPONSE_RDY, "Awaiting data transfer to SDRAM...\n\r");
        }
        else
        {
            stat = CLI_STATE_stream_qspi;
            cliPrintStr(RESPONSE_RDY, "Awaiting data transfer to QSPI...\n\r");
        }
    }
    else
    {
        /// TODO: Something went wrong
        char txBuf[CLI_TX_BUF_SIZE] = {0};
        sprintf(txBuf, "Parse error: unknown command <%s>\n\r", mainCmd);
        cliPrintStr(RESPONSE_ERR, txBuf);
    }
    return CLI_STAT_OK;
}

uint8_t cliErrHandler(void *err, uint32_t len)
{
}