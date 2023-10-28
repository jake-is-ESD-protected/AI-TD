#ifndef HAL_H
#define HAL_H

#define sampleRate 96000

void write2VCA(double value);

void doHalStuff();

void initHal();

void setLed(bool b);

void setFadingLed(double value);

bool readButton();

#endif