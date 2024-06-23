#pragma once

/// @brief
/// @param
void aiInit(void);

/// @brief
/// @param
void aiRun(float *input);

void aiReset();

void aiCancel();

float aiGetATTACK_T1(void);

float aiGetSUSTAIN_T1(void);

void aiProcess();

extern bool processAFFlag;
extern bool aiMode;
