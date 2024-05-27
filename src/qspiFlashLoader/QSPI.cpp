#include "daisy_seed.h"
#include "modelData.h"

using namespace daisy;

DaisySeed hw;

// #define FLAT_INSERT(COUNTER, SHAPE_IN, SHAPE_OUT, target, source)                                                                                                                                                                                                                                                                                                                                              \
//     do                                                                                                                                                                                                                                                                                                                                                                                                         \
//     {

// for (int i = 0; i < SHAPE_OUT; i++)
// {
//     std::vector<float> row;
//     for (int j = 0; j < SHAPE_IN; j++)
//     {
//         row.push_back((float)source[i][j]);
//     }
//     target.push_back(row);
// }
// }
// while (0)

#define IN_SHAPE 13

#define SHAPE_L_1 64
#define SHAPE_L_2 32
#define SHAPE_L_3 16
#define SHAPE_L_4 8
#define SHAPE_L_5 4
#define SHAPE_L_6 2

uint32_t DSY_QSPI_BSS weights_l1_buffer[SHAPE_L_1][IN_SHAPE];
// uint32_t DSY_QSPI_BSS bias_l1_buffer[SHAPE_L_1];

// uint32_t DSY_QSPI_BSS weights_l2_buffer[SHAPE_L_2][SHAPE_L_1];
// uint32_t DSY_QSPI_BSS bias_l2_buffer[SHAPE_L_2];

// uint32_t DSY_QSPI_BSS weights_l3_buffer[SHAPE_L_3][SHAPE_L_2];
// uint32_t DSY_QSPI_BSS bias_l3_buffer[SHAPE_L_3];

// uint32_t DSY_QSPI_BSS weights_l4_buffer[SHAPE_L_4][SHAPE_L_3];
// uint32_t DSY_QSPI_BSS bias_l4_buffer[SHAPE_L_4];

// uint32_t DSY_QSPI_BSS weights_l5_buffer[SHAPE_L_5][SHAPE_L_4];
// uint32_t DSY_QSPI_BSS bias_l5_buffer[SHAPE_L_5];

// uint32_t DSY_QSPI_BSS weights_l6_buffer[SHAPE_L_6][SHAPE_L_5];
// uint32_t DSY_QSPI_BSS bias_l6_buffer[SHAPE_L_6];

uint32_t *intermediateBuffer;

int main(void)
{
    hw.Configure();
    hw.Init();
    hw.SetLed(true);

    size_t size = sizeof(weights_l1);
    size_t address = (size_t)weights_l1_buffer;

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

    //

    /*size = sizeof(bias_l1);
    address = 0x90003898; //(size_t)bias_l1_buffer;

    // hw.qspi.Erase(address, address + size);
    hw.qspi.Write(address, size, (uint8_t *)bias_l1);

    // WEIGHTS FROM LAYER 1 END

    size = sizeof(weights_l2);
    address = 0x90001898; //(size_t)weights_l2_buffer;

    // hw.qspi.Erase(address, address + size);
    hw.qspi.Write(address, size, (uint8_t *)weights_l2);

    size = sizeof(bias_l2);
    address = 0x90001818; //(size_t)bias_l2_buffer;

    // hw.qspi.Erase(address, address + size);
    hw.qspi.Write(address, size, (uint8_t *)bias_l2);

    // WEIGHTS FROM LAYER 2 END

    size = sizeof(weights_l3);
    address = 0x90001018; //(size_t)weights_l3_buffer;

    // hw.qspi.Erase(address, address + size);
    hw.qspi.Write(address, size, (uint8_t *)weights_l3);

    size = sizeof(bias_l3);
    address = 0x90000fd8; //(size_t)bias_l3_buffer;

    // hw.qspi.Erase(address, address + size);
    hw.qspi.Write(address, size, (uint8_t *)bias_l3);

    // WEIGHTS FROM LAYER 3 END

    size = sizeof(weights_l4);
    address = 0x90000dd8; //(size_t)weights_l4_buffer;

    // hw.qspi.Erase(address, address + size);
    hw.qspi.Write(address, size, (uint8_t *)weights_l4);

    size = sizeof(bias_l4);
    address = 0x90000db8; //(size_t)bias_l4_buffer;

    // hw.qspi.Erase(address, address + size);
    hw.qspi.Write(address, size, (uint8_t *)bias_l4);

    // WEIGHTS FROM LAYER 4 END

    size = sizeof(weights_l5);
    address = 0x90000d38; //(size_t)weights_l5_buffer;

    // hw.qspi.Erase(address, address + size);
    hw.qspi.Write(address, size, (uint8_t *)weights_l5);

    size = sizeof(bias_l5);
    address = 0x90000d28; //(size_t)bias_l5_buffer;

    // hw.qspi.Erase(address, address + size);
    hw.qspi.Write(address, size, (uint8_t *)bias_l5);

    // WEIGHTS FROM LAYER 5 END

    size = sizeof(weights_l6);
    address = 0x90000d08; //(size_t) weights_l6_buffer;

    // hw.qspi.Erase(address, address + size);
    hw.qspi.Write(address, size, (uint8_t *)weights_l6);

    size = sizeof(bias_l6);
    address = 0x90000d00; //(size_t)bias_l6_buffer;

    // hw.qspi.Erase(address, address + size);
    hw.qspi.Write(address, size, (uint8_t *)bias_l6);*/

    // WEIGHTS FROM LAYER 6 END

    while (1)
    {
        hw.SetLed(true);
        System::Delay(1000);
        hw.SetLed(false);
        System::Delay(1000);
    }
}