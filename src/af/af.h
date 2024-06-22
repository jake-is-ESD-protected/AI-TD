/**
 * @file af.h
 * @author jake-is-ESD-protected
 * @date 2024-03-08
 * @brief Target device audio feature extraction algorithms for AI inputs.
 *
 *
 */

#ifndef _AF_H_
#define _AF_H_

#include <inttypes.h>
#include <stdbool.h>
#include "beatDetectionDSP/BTT.h"

#define ENV_SMOOTH_ORDER 4           // in n
#define ENV_SMOOTH_ATTACK 2          // in ms
#define ENV_SMOOTH_RELEASE 200       // in ms
#define ONSET_DETECTION_COMPENSATION_N (uint64_t)(20 * 0.001 * slowsampleRate) // The onset detection algorithm is about 10ms to late for proper attack detection

#define BEAT_DETECTION_BUFFER_SIZE 64
#define AUDIO_BUFFER_SIZE_S 8
#define AUDIO_BUFFER_SIZE slowsampleRate * AUDIO_BUFFER_SIZE_S
#define AUDIO_BUFF_FADE_TIME_S slowsampleRate * 0.04

#define MAX_ONSETS 4 * AUDIO_BUFFER_SIZE_S // 4 BPS IS 240 BPM

#define FFT_N2_LENGTH 512

// these values were obtained from explorative research of the data set
#define MAX_TEMPO           160.0
#define MAX_TA1_SAMPLES     20000.0
#define MAX_TA2_SAMPLES     80000.0
#define MAX_CENTROID        30000.0
#define MAX_CREST           20.0
#define MAX_FLUX            150.0

extern uint64_t audioBufferIndex;
extern uint64_t audioBufferRuntimeIndex;
extern uint64_t onsetBufferIndex;
extern dft_sample_t dftBuffer[BEAT_DETECTION_BUFFER_SIZE];

extern bool calculateAFFlag;
extern bool calculationsDoneFlag;
extern bool cancelationFlag;

void resetBuffer();

void initAf();

void EnvFollowerInit();

void BeatDetectionInit(); 

void AFInCAppend(float in);

void AFInCProcess();

void processBTTAndAFInC();

void processBTT();

/// @brief Get the envelope of a given signal.
/// @param sig Given signal.
/// @param len Length of given signal.
/// @param env Empty buffer for storage of derived envelope.
/// @attention Parameters for the envelope are fixed and defined above
void __afGetEnvelope(float *sig, float *env, uint64_t len);

/// @brief argmax() for C.
/// @param sig Given signal.
/// @param fromIdx Index to start the search from.
/// @param toIdx Index to stop the search at.
/// @return Index of highest value in interval.
uint64_t __afGetIdxOfMax(float *sig, uint64_t fromIdx, uint64_t toIdx);

/// @brief argmin() for C.
/// @param sig Given signal.
/// @param fromIdx Index to start the search from.
/// @param toIdx Index to stop the search at.
/// @return Index of lowest value in interval.
uint64_t __afGetIdxOfMin(float *sig, uint64_t fromIdx, uint64_t toIdx);

float afGetT1A();

float afGetT2A();

float afGetSpectralCentroid();

float afGetTempo();

float afGetPBandL();

float afGetPBandML();

float afGetPBandMH();

float afGetPBandH();

float afGetCrestFactor();

float afGetSpectralFlux();

void spectrumCalculatedCallback(float* mag, uint64_t N, float spectralFlux);

void EnvFollowerInitAf();
// --------------------------------------------------------------------------
// helper functions for dev debugging
// --------------------------------------------------------------------------

float __getAudioBuffer(void);

float __getEnvBuffer(void);

uint64_t __getOnsetBuffer(void);

float __getBeatMagnitude(int beatIndex);

#endif // _AF_H_