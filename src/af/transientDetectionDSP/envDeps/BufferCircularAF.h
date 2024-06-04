#pragma once

#define ENVELOPE_FOLLOWER_BUFFER_SIZE 1024

void BufferCircularInitAf(unsigned int num_samples);
void BufferCircularInit_power_of_twoAf(unsigned int num_samples_power_of_two);

void BufferCircularClearAf();

void BufferCircularPush_sampleAf(double input);
double BufferCircularReadAf(int samples_delay);

double BufferCircularMaxAf();