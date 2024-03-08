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

#define ENV_SMOOTH_ORDER 4            // in n
#define ENV_SMOOTH_ATTACK 2           // in ms
#define ENV_SMOOTH_RELEASE 200        // in ms
#define EXTREMA_SEARCH_INTERVAL 4000  // in samples
#define SAMPLE_RATE 96000 // TODO: import the fs macro used in the HAL (ask @mindlogger)

#define FRAME_LEN 1 // in s

/// @brief Get the envelope of a given signal.
/// @param sig Given signal.
/// @param len Length of given signal.
/// @param env Empty buffer for storage of derived envelope.
/// @attention Parameters for the envelope are fixed and defined above
void __afGetEnvelope(double* sig, double* env, uint32_t len);

/// @brief argmax() for C.
/// @param sig Given signal.
/// @param len Length of given signal.
/// @param fromIdx Index to start the search from.
/// @param toIdx Index to stop the search at.
/// @return Index of highest value in interval.
uint32_t __afGetIdxOfMax(double* sig, uint32_t len, uint32_t fromIdx, uint32_t toIdx);

/// @brief argmin() for C.
/// @param sig Given signal.
/// @param len Length of given signal.
/// @param fromIdx Index to start the search from.
/// @param toIdx Index to stop the search at.
/// @return Index of lowest value in interval.
uint32_t __afGetIdxOfMin(double* sig, uint32_t len, uint32_t fromIdx, uint32_t toIdx);

/// @brief Get the analytical attack time (rise time) of a given signal. 
/// @param sig Given signal.
/// @param len Length of given signal.
/// @param searchInterval Window size for search of peaks and valleys in the signal.
/// @param res Storage buffer for results (has size of 2 double).
/// @return Attack time.
void afGetTA(double* sig, uint32_t len, uint32_t searchInterval, double* res);


uint32_t afGetG1H(void);


uint32_t afGetG2H(void);


double afGetSpectralFlatnessDB(double* sig, uint32_t len);


double afGetCrestFactor(double* sig, uint32_t len);


double afGetSpectralCentroid(double* sig, uint32_t len);



#endif // _AF_H_