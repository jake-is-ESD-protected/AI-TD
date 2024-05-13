#include "BufferCircularAF.h"
#include <memory.h>

double EnvCircularBufferAf[ENVELOPE_FOLLOWER_BUFFER_SIZE];
unsigned int index_writeAf;
unsigned int len_bufferAf; // Must be nearest power of 2
unsigned int wrap_maskAf;  // Must be (len_buffer - 1)

void BufferCircularClearAf()
{
    memset(&EnvCircularBufferAf[0], 0, len_bufferAf * sizeof(double));
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

void BufferCircularPush_sampleAf(double input)
{
    EnvCircularBufferAf[index_writeAf++] = input;

    index_writeAf &= wrap_maskAf;
}

double BufferCircularReadAf(int samples_delay)
{
    int index_read = (index_writeAf - 1) - samples_delay;
    index_read &= wrap_maskAf;

    return EnvCircularBufferAf[index_read];
}

double BufferCircularMaxAf()
{
    double max = EnvCircularBufferAf[0];
    for (unsigned int i = 0; i < len_bufferAf; i++)
    {
        if (EnvCircularBufferAf[i] > max)
        {
            max = EnvCircularBufferAf[i];
        }
    }

    return max;
}