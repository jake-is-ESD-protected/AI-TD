#include "BufferCircular.h"
#include <memory.h>

double EnvCircularBuffer[ENVELOPE_FOLLOWER_BUFFER_SIZE];
unsigned int index_write;
unsigned int len_buffer; // Must be nearest power of 2
unsigned int wrap_mask;  // Must be (len_buffer - 1)

void BufferCircularClear()
{
    memset(&EnvCircularBuffer[0], 0, len_buffer * sizeof(double));
}

void BufferCircularInit(unsigned int num_samples)
{
    BufferCircularInit_power_of_two(ENVELOPE_FOLLOWER_BUFFER_SIZE);
}

void BufferCircularInit_power_of_two(unsigned int num_samples_power_of_two)
{
    index_write = 0;

    len_buffer = num_samples_power_of_two;

    wrap_mask = len_buffer - 1;

    BufferCircularClear();
}

void BufferCircularPush_sample(double input)
{
    EnvCircularBuffer[index_write++] = input;

    index_write &= wrap_mask;
}

double BufferCircularRead(int samples_delay)
{
    int index_read = (index_write - 1) - samples_delay;
    index_read &= wrap_mask;

    return EnvCircularBuffer[index_read];
}

double BufferCircularMax()
{
    double max = EnvCircularBuffer[0];
    for (unsigned int i = 0; i < len_buffer; i++)
    {
        if (EnvCircularBuffer[i] > max)
        {
            max = EnvCircularBuffer[i];
        }
    }

    return max;
}