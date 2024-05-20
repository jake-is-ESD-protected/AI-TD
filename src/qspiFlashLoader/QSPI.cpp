#include "daisy_seed.h"
#include "modelData.h"

using namespace daisy;

DaisySeed hw;

#define IN_SHAPE 13

#define SHAPE_L_1 64
#define SHAPE_L_2 32
#define SHAPE_L_3 16
#define SHAPE_L_4 8
#define SHAPE_L_5 4
#define SHAPE_L_6 2

uint64_t DSY_QSPI_BSS weights_l1_buffer[SHAPE_L_1][IN_SHAPE];
uint64_t DSY_QSPI_BSS bias_l1_buffer[SHAPE_L_1];

uint64_t DSY_QSPI_BSS weights_l2_buffer[SHAPE_L_2][SHAPE_L_1];
uint64_t DSY_QSPI_BSS bias_l2_buffer[SHAPE_L_2];

uint64_t DSY_QSPI_BSS weights_l3_buffer[SHAPE_L_3][SHAPE_L_2];
uint64_t DSY_QSPI_BSS bias_l3_buffer[SHAPE_L_3];

uint64_t DSY_QSPI_BSS weights_l4_buffer[SHAPE_L_4][SHAPE_L_3];
uint64_t DSY_QSPI_BSS bias_l4_buffer[SHAPE_L_4];

uint64_t DSY_QSPI_BSS weights_l5_buffer[SHAPE_L_5][SHAPE_L_4];
uint64_t DSY_QSPI_BSS bias_l5_buffer[SHAPE_L_5];

uint64_t DSY_QSPI_BSS weights_l6_buffer[SHAPE_L_6][SHAPE_L_5];
uint64_t DSY_QSPI_BSS bias_l6_buffer[SHAPE_L_6];

int main(void)
{
    hw.Configure();
    hw.Init();
    hw.SetLed(true);

    size_t size = sizeof(weights_l1);
    size_t address = (size_t)weights_l1_buffer;

    hw.qspi.Erase(address, address + size);
    hw.qspi.Write(address, size, (uint8_t *)weights_l1);

    size = sizeof(bias_l1);
    address = (size_t)bias_l1_buffer;

    hw.qspi.Erase(address, address + size);
    hw.qspi.Write(address, size, (uint8_t *)bias_l1);

    // WEIGHTS FROM LAYER 1 END

    size = sizeof(weights_l2);
    address = (size_t)weights_l2_buffer;

    hw.qspi.Erase(address, address + size);
    hw.qspi.Write(address, size, (uint8_t *)weights_l2);

    size = sizeof(bias_l2);
    address = (size_t)bias_l2_buffer;

    hw.qspi.Erase(address, address + size);
    hw.qspi.Write(address, size, (uint8_t *)bias_l2);

    // WEIGHTS FROM LAYER 2 END

    size = sizeof(weights_l3);
    address = (size_t)weights_l3_buffer;

    hw.qspi.Erase(address, address + size);
    hw.qspi.Write(address, size, (uint8_t *)weights_l3);

    size = sizeof(bias_l3);
    address = (size_t)bias_l3_buffer;

    hw.qspi.Erase(address, address + size);
    hw.qspi.Write(address, size, (uint8_t *)bias_l3);

    // WEIGHTS FROM LAYER 3 END

    size = sizeof(weights_l4);
    address = (size_t)weights_l4_buffer;

    hw.qspi.Erase(address, address + size);
    hw.qspi.Write(address, size, (uint8_t *)weights_l4);

    size = sizeof(bias_l4);
    address = (size_t)bias_l4_buffer;

    hw.qspi.Erase(address, address + size);
    hw.qspi.Write(address, size, (uint8_t *)bias_l4);

    // WEIGHTS FROM LAYER 4 END

    size = sizeof(weights_l5);
    address = (size_t)weights_l5_buffer;

    hw.qspi.Erase(address, address + size);
    hw.qspi.Write(address, size, (uint8_t *)weights_l5);

    size = sizeof(bias_l5);
    address = (size_t)bias_l5_buffer;

    hw.qspi.Erase(address, address + size);
    hw.qspi.Write(address, size, (uint8_t *)bias_l5);

    // WEIGHTS FROM LAYER 5 END

    size = sizeof(weights_l6);
    address = (size_t)weights_l6_buffer;

    hw.qspi.Erase(address, address + size);
    hw.qspi.Write(address, size, (uint8_t *)weights_l6);

    size = sizeof(bias_l6);
    address = (size_t)bias_l6_buffer;

    hw.qspi.Erase(address, address + size);
    hw.qspi.Write(address, size, (uint8_t *)bias_l6);

    // WEIGHTS FROM LAYER 6 END

    while (1)
    {
        hw.SetLed(true);
        System::Delay(500);
        hw.SetLed(false);
        System::Delay(500);
    }
}