#pragma once

#include <cmath>
#include <memory>

#define ENVELOPE_FOLLOWER_BUFFER_SIZE 1024

namespace bdsp
{

template <typename T> class BufferCircular
{
  public:
    BufferCircular();
    ~BufferCircular() = default;

    void init(unsigned int num_samples);
    void init_power_of_two(unsigned int num_samples_power_of_two);

    void clear();

    void push_sample(T input);
    T read(int samples_delay);
    T read(float samples_delay_fractional);

    T max();

  private:
    float buffer[ENVELOPE_FOLLOWER_BUFFER_SIZE];
    unsigned int index_write;
    unsigned int len_buffer; // Must be nearest power of 2
    unsigned int wrap_mask;  // Must be (len_buffer - 1)
};

} // namespace bdsp
