#ifndef HAL_H
#define HAL_H

#include "daisy_seed.h"

#define sampleRate 96000

void write2VCA(double value);

void initHal();

void setLed(bool b);

bool readButton();

void initTimer();

#endif