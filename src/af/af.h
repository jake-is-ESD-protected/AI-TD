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
#include "beatDetectionDSP/BTT.h"

#define ENV_SMOOTH_ORDER 4           // in n
#define ENV_SMOOTH_ATTACK 2          // in ms
#define ENV_SMOOTH_RELEASE 200       // in ms
#define ONSET_DETECTION_COMPENSATION_N (uint64_t)(20 * 0.001 * sampleRate) // The onset detection algorithm is about 10ms to late for proper attack detection

#define BEAT_DETECTION_BUFFER_SIZE 64
#define AUDIO_BUFFER_SIZE_S 8
#define AUDIO_BUFFER_SIZE sampleRate * AUDIO_BUFFER_SIZE_S
#define MAX_ONSETS 4 * AUDIO_BUFFER_SIZE_S // 4 BPS IS 240 BPM

#define FFT_N2_LENGTH 512

#define MAX_TEMPO           160.0
#define MAX_TA1             0.999
#define MAX_SPEC_FLATNESS   1000.0
#define MAX_CREST           100.0
#define MAX_FLUX            1000.0

extern uint64_t audioBufferIndex;
extern uint64_t audioBufferRuntimeIndex;
extern uint64_t onsetBufferIndex;
extern dft_sample_t dftBuffer[BEAT_DETECTION_BUFFER_SIZE];

void resetBuffer();

void initAf();

void EnvFollowerInit();

void BeatDetectionInit(); 

void AFInCAppend(double in);

void AFInCProcess();

/// @brief Get the envelope of a given signal.
/// @param sig Given signal.
/// @param len Length of given signal.
/// @param env Empty buffer for storage of derived envelope.
/// @attention Parameters for the envelope are fixed and defined above
void __afGetEnvelope(double *sig, double *env, uint64_t len);

/// @brief argmax() for C.
/// @param sig Given signal.
/// @param fromIdx Index to start the search from.
/// @param toIdx Index to stop the search at.
/// @return Index of highest value in interval.
uint64_t __afGetIdxOfMax(double *sig, uint64_t fromIdx, uint64_t toIdx);

/// @brief argmin() for C.
/// @param sig Given signal.
/// @param fromIdx Index to start the search from.
/// @param toIdx Index to stop the search at.
/// @return Index of lowest value in interval.
uint64_t __afGetIdxOfMin(double *sig, uint64_t fromIdx, uint64_t toIdx);

double afGetSpectralCentroid();

double afGetSpectralFlatness();

double afGetTempo();

double afGetPBandL();

double afGetPBandML();

double afGetPBandMH();

double afGetCrestFactor();

double afGetSpectralFlux();

void spectrumCalculatedCallback(float* mag, uint64_t N, float spectralFlux);

void EnvFollowerInitAf();
// --------------------------------------------------------------------------
// helper functions for dev debugging
// --------------------------------------------------------------------------

double __getAudioBuffer(void);

double __getEnvBuffer(void);

uint64_t __getOnsetBuffer(void);

double __getBeatMagnitude(int beatIndex);

#endif // _AF_H_