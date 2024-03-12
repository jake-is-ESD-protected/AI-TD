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

#define ENV_SMOOTH_ORDER 4           // in n
#define ENV_SMOOTH_ATTACK 2          // in ms
#define ENV_SMOOTH_RELEASE 400       // in ms
#define EXTREMA_SEARCH_INTERVAL 8000 // in samples
#define ONSET_DETECTION_COMPENSATION_N (uint32_t)(20 * 0.001 * sampleRate) // The onset detection algorithm is about 10ms to late for proper attack detection

#define FRAME_LEN 1 // in s

void resetBuffer();

void BeatDetectionInit();

void AFInCAppend(double in);

void AFInCProcess();

/// @brief Get the envelope of a given signal.
/// @param sig Given signal.
/// @param len Length of given signal.
/// @param env Empty buffer for storage of derived envelope.
/// @attention Parameters for the envelope are fixed and defined above
void __afGetEnvelope(double *sig, double *env, uint32_t len);

/// @brief argmax() for C.
/// @param sig Given signal.
/// @param fromIdx Index to start the search from.
/// @param toIdx Index to stop the search at.
/// @return Index of highest value in interval.
uint32_t __afGetIdxOfMax(double *sig, uint32_t fromIdx, uint32_t toIdx);

/// @brief argmin() for C.
/// @param sig Given signal.
/// @param fromIdx Index to start the search from.
/// @param toIdx Index to stop the search at.
/// @return Index of lowest value in interval.
uint32_t __afGetIdxOfMin(double *sig, uint32_t fromIdx, uint32_t toIdx);

/// @brief Get the analytical attack time (rise time) of a given signal.
/// @param sig Given signal.
/// @param fromIdx Index to start the search from.
/// @param toIdx Index to stop the search at.
/// @param searchInterval Window size for search of peaks and valleys in the signal.
/// @return Attack time.
void afGetTA(double *sig, uint32_t fromIdx, uint32_t toIdx, uint32_t searchInterval);

double afGetSpectralCentroid();

double afGetSpectralFlatnessDB();

double afGetTempo();

double afGetPBandL();

double afGetPBandML();

double afGetPBandMH();

double afGetCrestFactor();


// --------------------------------------------------------------------------
// helper functions for dev debugging
// --------------------------------------------------------------------------

double __getAudioBuffer(void);

double __getEnvBuffer(void);

uint32_t __getOnsetBuffer(void);

#endif // _AF_H_