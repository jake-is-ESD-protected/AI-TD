#include "daisy_seed.h"
#include "modelData.h"

using namespace daisy;

DaisySeed hw;

#define IN_SHAPE 13

#define QSPI_STORAGE_SPACE (IN_SHAPE * SHAPE_L_1 + SHAPE_L_1) + (SHAPE_L_1 * SHAPE_L_2 + SHAPE_L_2) + (SHAPE_L_2 * SHAPE_L_3 + SHAPE_L_3) + (SHAPE_L_3 * SHAPE_L_4 + SHAPE_L_4) + (SHAPE_L_4 * SHAPE_L_5 + SHAPE_L_5) + (SHAPE_L_5 * SHAPE_L_6 + SHAPE_L_6)
uint32_t DSY_QSPI_BSS weigthsQSPI[QSPI_STORAGE_SPACE];

uint32_t *intermediateBuffer;

int main(void)
{
    hw.Configure();
    hw.Init();
    hw.SetLed(true);

    size_t address = (size_t)weigthsQSPI;

    // clang-format off
    size_t totalSize =
    sizeof(weights_l1) + sizeof(bias_l1) +
    sizeof(weights_l2) + sizeof(bias_l2) +
    sizeof(weights_l3) + sizeof(bias_l3) +
    sizeof(weights_l4) + sizeof(bias_l4) +
    sizeof(weights_l5) + sizeof(bias_l5) +
    sizeof(weights_l6) + sizeof(bias_l6);
    // clang-format on

    intermediateBuffer = (uint32_t *)malloc(totalSize);

    uint32_t intermediateBufferCounter = 0;

    memcpy(&intermediateBuffer[intermediateBufferCounter], weights_l1, sizeof(weights_l1));
    intermediateBufferCounter += sizeof(weights_l1) / 4;

    memcpy(&intermediateBuffer[intermediateBufferCounter], bias_l1, sizeof(bias_l1));
    intermediateBufferCounter += sizeof(bias_l1) / 4;

    memcpy(&intermediateBuffer[intermediateBufferCounter], weights_l2, sizeof(weights_l2));
    intermediateBufferCounter += sizeof(weights_l2) / 4;

    memcpy(&intermediateBuffer[intermediateBufferCounter], bias_l2, sizeof(bias_l2));
    intermediateBufferCounter += sizeof(bias_l2) / 4;

    memcpy(&intermediateBuffer[intermediateBufferCounter], weights_l3, sizeof(weights_l3));
    intermediateBufferCounter += sizeof(weights_l3) / 4;

    memcpy(&intermediateBuffer[intermediateBufferCounter], bias_l3, sizeof(bias_l3));
    intermediateBufferCounter += sizeof(bias_l3) / 4;

    memcpy(&intermediateBuffer[intermediateBufferCounter], weights_l4, sizeof(weights_l4));
    intermediateBufferCounter += sizeof(weights_l4) / 4;

    memcpy(&intermediateBuffer[intermediateBufferCounter], bias_l4, sizeof(bias_l4));
    intermediateBufferCounter += sizeof(bias_l4) / 4;

    memcpy(&intermediateBuffer[intermediateBufferCounter], weights_l5, sizeof(weights_l5));
    intermediateBufferCounter += sizeof(weights_l5) / 4;

    memcpy(&intermediateBuffer[intermediateBufferCounter], bias_l5, sizeof(bias_l5));
    intermediateBufferCounter += sizeof(bias_l5) / 4;

    memcpy(&intermediateBuffer[intermediateBufferCounter], weights_l6, sizeof(weights_l6));
    intermediateBufferCounter += sizeof(weights_l6) / 4;

    memcpy(&intermediateBuffer[intermediateBufferCounter], bias_l6, sizeof(bias_l6));
    intermediateBufferCounter += sizeof(bias_l6) / 4;

    hw.qspi.Erase(address, address + totalSize);

    hw.qspi.Write(address, totalSize, (uint8_t *)intermediateBuffer);

    while (1)
    {
        hw.SetLed(true);
        System::Delay(1000);
        hw.SetLed(false);
        System::Delay(1000);
    }
}