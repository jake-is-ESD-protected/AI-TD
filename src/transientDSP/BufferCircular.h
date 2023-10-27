#ifndef BUFFER_CIRCULAR_H
#define BUFFER_CIRCULAR_H

#include <cmath>
#include <memory>

namespace bdsp
{

    template <typename T>
    class BufferCircular
    {
    public:
        BufferCircular();
        ~BufferCircular() = default;

        void init(unsigned int num_samples);
        void init_power_of_two(unsigned int num_samples_power_of_two);

        void clear();

        void push_sample(T input);
        T read(int samples_delay);
        T read(double samples_delay_fractional);

        T max();

    private:
        double buffer[512];
        unsigned int index_write;
        unsigned int len_buffer; // Must be nearest power of 2
        unsigned int wrap_mask;  // Must be (len_buffer - 1)
    };

} // namespace bdsp

#endif // BUFFER_CIRCULAR_H