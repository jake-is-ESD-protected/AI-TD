#pragma once

#define ENVELOPE_FOLLOWER_BUFFER_SIZE 1024

void BufferCircularInit(unsigned int num_samples);
void BufferCircularInit_power_of_two(unsigned int num_samples_power_of_two);

void BufferCircularClear();

void BufferCircularPush_sample(double input);
double BufferCircularRead(int samples_delay);

double BufferCircularMax();