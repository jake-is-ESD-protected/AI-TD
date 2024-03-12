/**
 * @file af.h
 * @author jake-is-ESD-protected
 * @date 2024-03-08
 * @brief Target device audio feature extraction algorithms for AI inputs.
 *
 *
 */

#include "af.h"
#include "transientDetectionDSP/Smoother4Exponential.h"
#include "../hal/globalDefinitions.hpp"
#include "beatDetectionDSP/BTT.h"
#include <inttypes.h>
#include <math.h>
#include <stdio.h>

BTT *btt;
#define BEAT_DETECTION_BUFFER_SIZE 64
#define AUDIO_BUFFER_SIZE_S 8
#define AUDIO_BUFFER_SIZE sampleRate * AUDIO_BUFFER_SIZE_S
#define MAX_ONSETS 4 * AUDIO_BUFFER_SIZE_S // 4 BPS IS 240 BPM
__attribute__((section(".sdram_bss"))) double audioBuffer[AUDIO_BUFFER_SIZE];
__attribute__((section(".sdram_bss"))) double envBuffer[AUDIO_BUFFER_SIZE];
__attribute__((section(".sdram_bss"))) uint32_t onsetBuffer[MAX_ONSETS];
uint64_t audioBufferIndex = 0;
uint64_t audioBufferRuntimeIndex = 0;
uint64_t onsetBufferIndex = 0;
dft_sample_t buffer[BEAT_DETECTION_BUFFER_SIZE];

double T1A = 0;
double T2A = 0;

void resetBuffer()
{
    audioBufferIndex = 0;
    audioBufferRuntimeIndex = 0;
    onsetBufferIndex = 0;
    T1A = 0;
    T2A = 0;
}

void onset_detected_callback(void *SELF, unsigned long long sample_time)
{
    if(onsetBufferIndex < MAX_ONSETS)
    {
        onsetBuffer[onsetBufferIndex] = audioBufferRuntimeIndex;
        onsetBufferIndex++;
    }
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
    btt_set_onset_tracking_callback(btt, onset_detected_callback, NULL);
}

void AFInCAppend(double in)
{
    if (audioBufferIndex < AUDIO_BUFFER_SIZE)
    {
        audioBuffer[audioBufferIndex] = in;
        audioBufferIndex++;
    }
}

void AFInCProcess()
{
    for (uint64_t i = 0; i < audioBufferIndex; i++)
    {
        buffer[0] = audioBuffer[i];
        btt_process(btt, buffer, 1);
        audioBufferRuntimeIndex++;
    }

    for (uint64_t i = 0; i < onsetBufferIndex-1; i++)
    {
        if(onsetBuffer[i] > ONSET_DETECTION_COMPENSATION_N)
        {
            onsetBuffer[i] -= ONSET_DETECTION_COMPENSATION_N;
        }

        afGetTA(audioBuffer, onsetBuffer[i], onsetBuffer[i+1], onsetBuffer[i+1] - onsetBuffer[i]);
    }
    T1A = T1A / (onsetBufferIndex - 1);
    T2A = T2A / (onsetBufferIndex - 1);
}

void __afGetEnvelope(double *sig, double *env, uint32_t len)
{
    initAll4();
    setAttackAll4(ENV_SMOOTH_ATTACK);
    setReleaseAll4(ENV_SMOOTH_RELEASE);

    for (uint32_t i = 0; i < len; i++)
    {
        *env = processAll4(*sig);
        sig++;
        env++;
    }
}

uint32_t __afGetIdxOfMax(double *sig, uint32_t fromIdx, uint32_t toIdx)
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

uint32_t __afGetIdxOfMin(double *sig, uint32_t fromIdx, uint32_t toIdx)
{
    uint32_t idxMin = fromIdx;
    for (uint32_t i = fromIdx + 1; i < toIdx; i++)
    {
        if (sig[i] < sig[idxMin])
        {
            idxMin = i;
        }
        else if (sig[i] > sig[i - 1]) 
        {
            break;
        }
    }
    return idxMin;
}

void afGetTA(double *sig, uint32_t fromIdx, uint32_t toIdx, uint32_t searchInterval)
{
    __afGetEnvelope(&sig[fromIdx], &envBuffer[fromIdx], toIdx - fromIdx);

    uint32_t idxMax = __afGetIdxOfMax(envBuffer, fromIdx, toIdx);

    uint32_t start = 0;
    uint32_t stop = 0;
    if ((idxMax - searchInterval) < fromIdx)
    {
        start = fromIdx;
    }
    else
    {
        start = idxMax - searchInterval;
    }
    uint32_t idxMinPre = __afGetIdxOfMin(envBuffer, start, idxMax);
    
    if ((idxMax + searchInterval) > toIdx)
    {
        stop = toIdx;
    }
    else
    {
        stop = idxMax + searchInterval;
    }
    uint32_t idxMinPost = __afGetIdxOfMin(envBuffer, idxMax, stop);

    T1A += ( (double) (idxMax - idxMinPre) / (double) sampleRate);
    T2A += ( (double) (idxMinPost - idxMax) / (double) sampleRate);
}

// clang-format off
double afGetT1A() {
    return T1A;
}

double afGetT2A() {
    return T2A;
}

double afGetSpectralCentroid()
{

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

    for(int i = 0; i < audioBufferRuntimeIndex;i++)
    {
        abs_sample = abs(buffer[i]);
        if(abs_sample > max_sample)
            max_sample = abs_sample;
    }

    for(int i = 0; i < audioBufferRuntimeIndex;i++)
        rms += buffer[i] * buffer[i];
    rms = rms / audioBufferRuntimeIndex;
    rms = pow(rms, 0.5);

    if(rms == 0.0)
        return 0.0;

    return max_sample / rms;
}


// --------------------------------------------------------------------------
// helper functions for dev debugging
// --------------------------------------------------------------------------

uint64_t __audioIndex = 0;
double __getAudioBuffer(void){
    double sample = audioBuffer[__audioIndex];
    __audioIndex++;
    if(__audioIndex == AUDIO_BUFFER_SIZE) { __audioIndex = 0; }
    return sample; 
}

uint64_t __envIndex = 0;
double __getEnvBuffer(void){
    double sample = envBuffer[__envIndex];
    __envIndex++;
    if(__envIndex == AUDIO_BUFFER_SIZE) { __envIndex = 0; }
    return sample; 
}

uint64_t __onsetIndex = 0;
uint32_t __getOnsetBuffer(void){
    double sample = onsetBuffer[__onsetIndex];
    __onsetIndex++;
    if(__onsetIndex == MAX_ONSETS) { __onsetIndex = 0; }
    return sample; 
}

void __resetIndexDebug(void){
    __audioIndex = 0;
    __envIndex = 0;
    __onsetIndex = 0;
}