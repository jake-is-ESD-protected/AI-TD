#include "BufferCircular.h"
#include "mappings.h"
#include <memory.h>

namespace bdsp
{

template <typename T> BufferCircular<T>::BufferCircular() : index_write(0), len_buffer(ENVELOPE_FOLLOWER_BUFFER_SIZE), wrap_mask(ENVELOPE_FOLLOWER_BUFFER_SIZE - 1)
{
}

template <typename T> void BufferCircular<T>::clear()
{
    memset(&buffer[0], 0, len_buffer * sizeof(T));
}

template <typename T> void BufferCircular<T>::init(unsigned int num_samples)
{
    init_power_of_two(ENVELOPE_FOLLOWER_BUFFER_SIZE);
}

template <typename T> void BufferCircular<T>::init_power_of_two(unsigned int num_samples_power_of_two)
{
    index_write = 0;

    len_buffer = num_samples_power_of_two;

    wrap_mask = len_buffer - 1;

    clear();
}

template <typename T> void BufferCircular<T>::push_sample(T input)
{
    buffer[index_write++] = input;

    index_write &= wrap_mask;
}

template <typename T> T BufferCircular<T>::read(int samples_delay)
{
    int index_read = (index_write - 1) - samples_delay;
    index_read &= wrap_mask;

    return buffer[index_read];
}

template <typename T> T BufferCircular<T>::read(float samples_delay_fractional)
{
    T y1 = read((int)samples_delay_fractional);
    T y2 = read((int)samples_delay_fractional + 1);

    float fraction = samples_delay_fractional - (int)samples_delay_fractional;

    return mappings::map_linear_norm<T>(fraction, y1, y2);
}

template <typename T> T BufferCircular<T>::max()
{
    float max = buffer[0];
    for (unsigned int i = 0; i < len_buffer; i++)
    {
        if (buffer[i] > max)
        {
            max = buffer[i];
        }
    }

    return max;
}

template class BufferCircular<float>;

} // namespace bdsp