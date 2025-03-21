#include "BufferCircularAF.h"
#include <memory.h>

float __attribute__((section(".sdram_bss"))) EnvCircularBufferAf[ENVELOPE_FOLLOWER_BUFFER_SIZE];
unsigned int index_writeAf;
unsigned int len_bufferAf; // Must be nearest power of 2
unsigned int wrap_maskAf;  // Must be (len_buffer - 1)

void BufferCircularClearAf()
{
    memset(&EnvCircularBufferAf[0], 0, len_bufferAf * sizeof(float));
}

void BufferCircularInitAf(unsigned int num_samples)
{
    BufferCircularInit_power_of_twoAf(ENVELOPE_FOLLOWER_BUFFER_SIZE);
}

void BufferCircularInit_power_of_twoAf(unsigned int num_samples_power_of_two)
{
    index_writeAf = 0;

    len_bufferAf = num_samples_power_of_two;

    wrap_maskAf = len_bufferAf - 1;

    BufferCircularClearAf();
}

void BufferCircularPush_sampleAf(float input)
{
    EnvCircularBufferAf[index_writeAf++] = input;

    index_writeAf &= wrap_maskAf;
}

float BufferCircularReadAf(int samples_delay)
{
    int index_read = (index_writeAf - 1) - samples_delay;
    index_read &= wrap_maskAf;

    return EnvCircularBufferAf[index_read];
}

float BufferCircularMaxAf()
{
    float max = EnvCircularBufferAf[0];
    for (unsigned int i = 0; i < len_bufferAf; i++)
    {
        if (EnvCircularBufferAf[i] > max)
        {
            max = EnvCircularBufferAf[i];
        }
    }

    return max;
}