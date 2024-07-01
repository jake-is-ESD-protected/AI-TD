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

uint32_t aiGetTimeInf(void);

extern bool processAFFlag;
extern bool aiMode;

extern uint32_t time_inf;