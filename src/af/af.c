/**
 * @file af.h
 * @author jake-is-ESD-protected
 * @date 2024-03-08
 * @brief Target device audio feature extraction algorithms for AI inputs.
 *
 *
 */

#include "af.h"
#include "transientDetectionDSP/SmootherExponential.h"
#include "../hal/globalDefinitions.hpp"
#include "beatDetectionDSP/BTT.h"
#include <inttypes.h>
#include <math.h>
#include <stdio.h>

BTT *btt;
#define BEAT_DETECTION_BUFFER_SIZE 64
#define AUDIO_BUFFER_SIZE sampleRate * 8
__attribute__((section(".sdram_bss"))) double audioBuffer[AUDIO_BUFFER_SIZE];
uint64_t audioBufferPointer = 0;
dft_sample_t buffer[BEAT_DETECTION_BUFFER_SIZE];

double T1A = 0;
double T2A = 0;

void resetBuffer()
{
    audioBufferPointer = 0;
}

void BeatDetectionInit()
{
    // clang-format off
    btt = btt_new(BTT_SUGGESTED_SPECTRAL_FLUX_STFT_LEN,
                    BTT_SUGGESTED_SPECTRAL_FLUX_STFT_OVERLAP,
                    BTT_SUGGESTED_OSS_FILTER_ORDER,
                    BTT_SUGGESTED_OSS_LENGTH,
                    BTT_SUGGESTED_ONSET_THRESHOLD_N,
                    BTT_SUGGESTED_CBSS_LENGTH,
                    sampleRate,
                    BTT_DEFAULT_ANALYSIS_LATENCY_ONSET_ADJUSTMENT,
                    BTT_DEFAULT_ANALYSIS_LATENCY_BEAT_ADJUSTMENT
                    );
    // clang-format on
    btt_set_tracking_mode(btt, BTT_ONSET_AND_TEMPO_AND_BEAT_TRACKING);
}

void AFInCAppend(double in)
{
    if (audioBufferPointer < AUDIO_BUFFER_SIZE)
    {
        audioBuffer[audioBufferPointer] = in;
        audioBufferPointer++;
    }
}

void AFInCProcess()
{
    for (uint64_t i = 0; i < audioBufferPointer; i++)
    {
        buffer[0] = audioBuffer[i];
        btt_process(btt, buffer, 1);
    }
}

void __afGetEnvelope(double *sig, double *env, uint32_t len)
{
    SmootherExponential expSmooth;
    expSmooth.init(ENV_SMOOTH_ORDER);
    expSmooth.reset(sampleRate);
    expSmooth.set_attack(ENV_SMOOTH_ATTACK);
    expSmooth.set_release(ENV_SMOOTH_RELEASE);

    for (uint32_t i = 0; i < len; i++)
    {
        env[i] = expSmooth.process(sig[i]);
    }
}

uint32_t __afGetIdxOfMax(double *sig, uint32_t len, uint32_t fromIdx, uint32_t toIdx)
{
    uint32_t idxMax = fromIdx;
    for (uint32_t i = fromIdx; i < toIdx; i++)
    {
        if (sig[i] > sig[idxMax])
        {
            idxMax = i;
        }
    }
    return idxMax;
}

uint32_t __afGetIdxOfMin(double *sig, uint32_t len, uint32_t fromIdx, uint32_t toIdx)
{
    uint32_t idxMin = fromIdx;
    for (uint32_t i = fromIdx; i < toIdx; i++)
    {
        if (sig[i] < sig[idxMin])
        {
            idxMin = i;
        }
    }
    return idxMin;
}

void afGetTA(double *sig, uint32_t len, uint32_t searchInterval, double *res)
{
    uint32_t nFrames = len / (FRAME_LEN * sampleRate);
    for (uint32_t i = 0; i < nFrames; i++)
    {
        uint32_t lBound = i * sampleRate;
        uint32_t uBound = (i + 1) * sampleRate;
        uint32_t idxMax = __afGetIdxOfMax(sig, len, lBound, uBound);

        uint32_t start = 0;
        uint32_t stop = 0;
        if (idxMax < searchInterval)
        {
            start = lBound;
        }
        else
        {
            start = idxMax - searchInterval;
        }
        uint32_t idxMinPre = __afGetIdxOfMin(sig, len, start, idxMax);

        if ((idxMax + searchInterval) > uBound)
        {
            stop = uBound;
        }
        else
        {
            stop = idxMax + searchInterval;
        }
        uint32_t idxMinPost = __afGetIdxOfMin(sig, len, idxMax, stop);
        T1A += ((idxMax - idxMinPre) / sampleRate);
        T2A += ((idxMinPost - idxMax) / sampleRate);
    }
    T1A = T1A / nFrames;
    T2A = T2A / nFrames;
}

// clang-format off
double afGetT1A() {
    return T1A;
}

double afGetT2A() {
    return T2A;
}

double afGetSpectralFlatnessDB() {
    return 1;
}

double afGetTempo() {
    return btt_get_tempo_bpm(btt);
}

double afGetPBandL() {
    return 1;
}
double afGetPBandML() {
    return 1;
}
double afGetPBandMH() {
    return 1;
}

double afGetCrestFactor() {
    double max_sample = 0.0;
    double rms = 0.0;
    double abs_sample = 0.0;

    for(int i = 0; i < audioBufferPointer;i++)
    {
        abs_sample = abs(buffer[i]);
        if(abs_sample > max_sample)
            max_sample = abs_sample;
    }

    for(int i = 0; i < audioBufferPointer;i++)
        rms += buffer[i] * buffer[i];
    rms = rms / audioBufferPointer;
    rms = pow(rms, 0.5);

    if(rms == 0.0)
        return 0.0;

    return max_sample / rms;
}