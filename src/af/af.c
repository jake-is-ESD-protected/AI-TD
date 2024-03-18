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
#include "transientDetectionDSP/EnvelopeFollowerPeakHold.h"
#include "percentileCalculator/percentileCalculator.h"
#include "../hal/globalDefinitions.hpp"
#include "beatDetectionDSP/BTT.h"
#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

BTT *btt;
#define BEAT_DETECTION_BUFFER_SIZE 64
#define AUDIO_BUFFER_SIZE_S 8
#define AUDIO_BUFFER_SIZE sampleRate * AUDIO_BUFFER_SIZE_S
#define MAX_ONSETS 4 * AUDIO_BUFFER_SIZE_S // 4 BPS IS 240 BPM

__attribute__((section(".sdram_bss"))) double audioBuffer[AUDIO_BUFFER_SIZE];
__attribute__((section(".sdram_bss"))) double envBuffer[AUDIO_BUFFER_SIZE];

__attribute__((section(".sdram_bss"))) uint64_t onsetBuffer[MAX_ONSETS];

__attribute__((section(".sdram_bss"))) double onsetT1ABuffer[MAX_ONSETS];
__attribute__((section(".sdram_bss"))) double onsetT2ABuffer[MAX_ONSETS];

uint64_t audioBufferIndex = 0;
uint64_t audioBufferRuntimeIndex = 0;
uint64_t onsetBufferIndex = 0;
dft_sample_t buffer[BEAT_DETECTION_BUFFER_SIZE];

double T1A = 0;
double T2A = 0;

void resetBuffer()
{
    memset(audioBuffer,0, sizeof(double) * AUDIO_BUFFER_SIZE);
    memset(envBuffer,0, sizeof(double) * AUDIO_BUFFER_SIZE);
    memset(onsetBuffer,0, sizeof(uint64_t) * MAX_ONSETS);
    memset(onsetT1ABuffer,0, sizeof(double) * MAX_ONSETS);
    memset(onsetT2ABuffer,0, sizeof(double) * MAX_ONSETS);
    memset(buffer, 0, sizeof(dft_sample_t) * BEAT_DETECTION_BUFFER_SIZE);
    audioBufferIndex = 0;
    audioBufferRuntimeIndex = 0;
    onsetBufferIndex = 0;
}

void onset_detected_callback(void *SELF, unsigned long long sample_time)
{
    if(onsetBufferIndex < MAX_ONSETS)
    {
        onsetBuffer[onsetBufferIndex] = audioBufferRuntimeIndex;
        onsetBufferIndex++;
    }
}

void initAf()
{
    BeatDetectionInit();
    EnvFollowerInit();
}

void EnvFollowerInit()
{
    EnvelopeFollowerPeakHoldInit(1);
    initAll4();
    setAttackAll4(ENV_SMOOTH_ATTACK);
    setReleaseAll4(ENV_SMOOTH_RELEASE);
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
        envBuffer[i] = processEnvelope(EnvelopeFollowerPeakHoldProcess(audioBuffer[i]));
        audioBufferRuntimeIndex++;
    }

    //TODO:
    //Spectral Flattness (per frame) -- FREQUENCY DOMAIN
    //Spectral Centroid (per frame) -- FREQUENCY DOMAIN
    //Spectral flux (per frame) -- FREQUENCY DOMAIN
    //4 BAND EQ -- (per frame) -- FREQUENCY DOMAIN

    /*
    OK HERES IS HOW WE GONNA DO IT
    FFT_SIZE IS 2048 FOR DAISY REASONS
    MAX FRAME IS HALF AUDIO BUFFER (4) * sample_rate = 384000
    384000 / 2048 = 187.5 IS THE MAX AMOUNT OF WINDOWS PER MAX FRAME SIZE

    WE TRY TO MAKE A BUFFER IN STFT.h and read it here
    THEN PROCESS AF FROM IT

    */
    //#define TARGET_DEVICE_DAISY 0

    #ifdef TARGET_DEVICE_DAISY

    arm_cfft_f32(&arm_cfft_sR_f32_len2048, input, 0 /*ifftFlag*/, 1 /*doBitReverse*/);
    arm_cmplx_mag_f32(input, output, FFT_SIZE /*fftSize*/);

    #else
    //TODO:
    /*
    SOME IMPLEMENTATION THAT WORKS ON ALL PLATFORMS
    dft_real_forward_dft(self->real, self->imag, self->fft_N); //FROM BEATDETECTION
    //IN PLACE WE NEED TO GIVE IMAG BUFFER WITH 0s

    //BUILD A FCUNTION FOR MAG VALUES!
    
    */
    #endif



    //Crest Factor (whole buffer) -- TIME DOMAIN

    for (uint64_t i = 0; i < onsetBufferIndex-1; i++) //COMPENSATE ALL ONSETS FIRST
    {
        if(onsetBuffer[i] > ONSET_DETECTION_COMPENSATION_N)
        {
            onsetBuffer[i] -= ONSET_DETECTION_COMPENSATION_N;
        }
    }

    for (uint64_t i = 0; i < onsetBufferIndex-1; i++)
    {
        uint64_t idxMax = __afGetIdxOfMax(envBuffer, onsetBuffer[i], onsetBuffer[i+1]);

        uint64_t idxMinPre = __afGetIdxOfMin(envBuffer, onsetBuffer[i], idxMax);

        uint64_t idxMinPost = __afGetIdxOfMin(envBuffer, idxMax, onsetBuffer[i+1]);

        onsetT1ABuffer[i] = idxMax - idxMinPre;
        onsetT2ABuffer[i] = idxMinPost - idxMax;
    }

    T1A = findPercentile(onsetT1ABuffer, onsetBufferIndex, 75);
    T2A = findPercentile(onsetT2ABuffer, onsetBufferIndex, 75);
}


uint64_t __afGetIdxOfMax(double *sig, uint64_t fromIdx, uint64_t toIdx)
{
    uint64_t idxMax = fromIdx;
    for (uint64_t i = fromIdx; i < toIdx; i++)
    {
        if (sig[i] > sig[idxMax])
        {
            idxMax = i;
        }
    }
    return idxMax;
}

uint64_t __afGetIdxOfMin(double *sig, uint64_t fromIdx, uint64_t toIdx)
{
    uint64_t idxMin = fromIdx;
    for (uint64_t i = fromIdx; i < toIdx; i++)
    {
        if (sig[i] < sig[idxMin])
        {
            idxMin = i;
        }
    }
    return idxMin;
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
    return 1;
}

double afGetSpectralFlatnessDB() {
    return 1;
}

double afGetTempo() {
    //TODO:
    //EVERYTHING ABOVE 160 IS //2
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
uint64_t __getOnsetBuffer(void){
    double sample = onsetBuffer[__onsetIndex];
    __onsetIndex++;
    if(__onsetIndex == MAX_ONSETS) { __onsetIndex = 0; }
    return sample; 
}

uint64_t __TA1Index = 0;
uint64_t __getTA1Buffer(void){
    double sample = onsetT1ABuffer[__TA1Index];
    __TA1Index++;
    if(__TA1Index == MAX_ONSETS) { __TA1Index = 0; }
    return sample; 
}

uint64_t __TA2Index = 0;
uint64_t __getTA2Buffer(void){
    double sample = onsetT2ABuffer[__TA2Index];
    __TA2Index++;
    if(__TA2Index == MAX_ONSETS) { __TA2Index = 0; }
    return sample; 
}

void __resetIndexDebug(void){
    __audioIndex = 0;
    __envIndex = 0;
    __onsetIndex = 0;
    __TA1Index = 0;
    __TA2Index = 0;
}