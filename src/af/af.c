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
#include "audioFeatureDSP/audioFeatureDSP.h"
#include "percentileCalculator/percentileCalculator.h"
#include "../hal/globalDefinitions.hpp"
#include "beatDetectionDSP/BTT.h"
#include <inttypes.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

BTT *btt;

__attribute__((section(".sdram_bss"))) double audioBuffer[AUDIO_BUFFER_SIZE];
__attribute__((section(".sdram_bss"))) double envBuffer[AUDIO_BUFFER_SIZE];

__attribute__((section(".sdram_bss"))) uint64_t onsetBuffer[MAX_ONSETS];

__attribute__((section(".sdram_bss"))) double onsetT1ABuffer[MAX_ONSETS];
__attribute__((section(".sdram_bss"))) double onsetT2ABuffer[MAX_ONSETS];

__attribute__((section(".sdram_bss"))) float magnitudeBuffer[FFT_N2_LENGTH];

__attribute__((section(".sdram_bss"))) double spectralFluxBuffer[MAX_ONSETS];
__attribute__((section(".sdram_bss"))) double spectralCentroidBuffer[MAX_ONSETS];
__attribute__((section(".sdram_bss"))) double spectralFlatnessBuffer[MAX_ONSETS];
__attribute__((section(".sdram_bss"))) double bandLBuffer[MAX_ONSETS];
__attribute__((section(".sdram_bss"))) double bandMLBuffer[MAX_ONSETS];
__attribute__((section(".sdram_bss"))) double bandMHBuffer[MAX_ONSETS];
__attribute__((section(".sdram_bss"))) double bandHBuffer[MAX_ONSETS];
__attribute__((section(".sdram_bss"))) double crestFactorBuffer[MAX_ONSETS];

__attribute__((section(".sdram_bss"))) double magnitudeBeatBuffer[MAX_ONSETS][FFT_N2_LENGTH];

uint64_t audioBufferIndex = 0;
uint64_t audioBufferRuntimeIndex = 0;
uint64_t onsetBufferIndex = 0;
uint64_t spectralFluxIndex = 0;
dft_sample_t dftBuffer[BEAT_DETECTION_BUFFER_SIZE];

bool firstSepctrumFlag = true;

double spectralCentroid = 0;
double spectralFlatness = 0;
double BandL = 0; //150Hz
double BandML = 0; //800Hz
double BandMH = 0; //2kHz
double BandH = 0; //20kHz
double crestFactor = 0;
double spectralFlux = 0;
double T1A = 0;
double T2A = 0;

void resetBuffer()
{
    memset(audioBuffer,0, sizeof(double) * AUDIO_BUFFER_SIZE);
    memset(envBuffer,0, sizeof(double) * AUDIO_BUFFER_SIZE);
    memset(onsetBuffer,0, sizeof(uint64_t) * MAX_ONSETS);
    memset(onsetT1ABuffer,0, sizeof(double) * MAX_ONSETS);
    memset(onsetT2ABuffer,0, sizeof(double) * MAX_ONSETS);
    memset(dftBuffer, 0, sizeof(dft_sample_t) * BEAT_DETECTION_BUFFER_SIZE);
    audioBufferIndex = 0;
    audioBufferRuntimeIndex = 0;
    onsetBufferIndex = 0;
    firstSepctrumFlag = true;
    spectralFluxIndex = 0;
}

void onset_detected_callback(void *SELF, unsigned long long sample_time)
{
    if(onsetBufferIndex < MAX_ONSETS)
    {
        onsetBuffer[onsetBufferIndex] = audioBufferRuntimeIndex;
        for(int i = 0; i < FFT_N2_LENGTH; i++)
        {
            magnitudeBeatBuffer[onsetBufferIndex][i] = magnitudeBuffer[i];
        }
        firstSepctrumFlag = true;
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

void spectrumCalculatedCallback(float* mag, uint64_t N, float spectralFlux)
{
    spectralFluxBuffer[spectralFluxIndex] = spectralFlux;
    if(spectralFluxIndex < MAX_ONSETS)
        spectralFluxIndex++;

    if(N != FFT_N2_LENGTH)
        return; //TODO: ERROR HANDLING

    if(firstSepctrumFlag)
    {
        for(int i = 0; i < N; i ++)
        {
            magnitudeBuffer[i] = fabs(mag[i]);
        }
        firstSepctrumFlag = false;
    }
    else
    {
        for(int i = 0; i < N; i ++)
        {
            magnitudeBuffer[i] = (magnitudeBuffer[i] + mag[i]) / 2.0;
        }
    }
}

void AFInCProcess()
{
    double audioBufferMax = 0;
    for (uint64_t i = 0; i < audioBufferIndex; i++)
    {
            if (fabs(audioBuffer[i]) > audioBufferMax) {
                audioBufferMax = fabs(audioBuffer[i]);
        }
    }
    double audioBufferNormalizationFactor = 1.0f / audioBufferMax;
    for (uint64_t i = 0; i < audioBufferIndex; i++)
    {
        audioBuffer[i] *= audioBufferNormalizationFactor;
    }
    
    for (uint64_t i = 0; i < audioBufferIndex; i++)
    {
        dftBuffer[0] = audioBuffer[i];
        btt_process(btt, dftBuffer, 1);
        envBuffer[i] = processEnvelope(EnvelopeFollowerPeakHoldProcess(audioBuffer[i]));
        audioBufferRuntimeIndex++;
    }

    for (uint64_t i = 0; i < onsetBufferIndex-1; i++) //COMPENSATE ALL ONSETS FIRST
    {
        if(onsetBuffer[i] > ONSET_DETECTION_COMPENSATION_N)
        {
            onsetBuffer[i] -= ONSET_DETECTION_COMPENSATION_N;
        }
    }

    double magBufferMax = 0;
    for(uint64_t i = 0; i < onsetBufferIndex-1; i++) //FIND MAX
    {
        for(int j = 0; j < FFT_N2_LENGTH; j++)
        {
            if (fabs(magnitudeBeatBuffer[i][j]) > magBufferMax)
            {
                magBufferMax = fabs(magnitudeBeatBuffer[i][j]);  
            }
        }
    }

    double magBufferNormalizationFactor = 1.0f / magBufferMax;
    for (uint64_t i = 0; i < onsetBufferIndex; i++) //NORMALIZE ALL THE BEAT BUFFERS
    {
        for(int j = 0; j < FFT_N2_LENGTH; j++)
        {
            magnitudeBeatBuffer[i][j] *= magBufferNormalizationFactor;
        }
    }

    for (uint64_t i = 0; i < onsetBufferIndex-1; i++)
    {
        uint64_t idxMax = __afGetIdxOfMax(envBuffer, onsetBuffer[i], onsetBuffer[i+1]);

        uint64_t idxMinPre = __afGetIdxOfMin(envBuffer, onsetBuffer[i], idxMax);

        uint64_t idxMinPost = __afGetIdxOfMin(envBuffer, idxMax, onsetBuffer[i+1]);

        onsetT1ABuffer[i] = idxMax - idxMinPre;
        onsetT2ABuffer[i] = idxMinPost - idxMax;

        //FETCH AF HERE FROM FUNCTION IDENTICAL TO ABOVE
        spectralCentroidBuffer[i] = calculateSpectralCentroid(i);
        spectralFlatnessBuffer[i] = calculateSpectralFlatness(i);
        bandLBuffer[i] = calculateBandL(i);
        bandMLBuffer[i] = calculateBandML(i);
        bandMHBuffer[i] = calculateBandMH(i);
        bandHBuffer[i] = calculateBandH(i);
        crestFactorBuffer[i] = calculateCrestFactor(audioBuffer, onsetBuffer[i], onsetBuffer[i+1] - onsetBuffer[i]);

    }
    spectralCentroid = findPercentile(spectralCentroidBuffer, onsetBufferIndex, 75); 
    spectralFlatness = findPercentile(spectralFlatnessBuffer, onsetBufferIndex, 75);
    BandL = findPercentile(bandLBuffer, onsetBufferIndex, 75);
    BandML = findPercentile(bandMLBuffer, onsetBufferIndex, 75);
    BandMH = findPercentile(bandMHBuffer, onsetBufferIndex, 75);
    BandH = findPercentile(bandHBuffer, onsetBufferIndex, 75);
    crestFactor = findPercentile(crestFactorBuffer, onsetBufferIndex, 75);
    spectralFlux = findPercentile(spectralFluxBuffer, spectralFluxIndex, 75);
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
    return spectralCentroid;
}


double afGetSpectralFlatness()
{
    return spectralFlatness;
}

double afGetTempo() {
    //TODO:
    //EVERYTHING ABOVE 160 IS //2
    return btt_get_tempo_bpm(btt);
}

double afGetPBandL() {
    return BandL;
}

double afGetPBandML() {
    return BandML;
}

double afGetPBandMH() {
    return BandMH;
}

double afGetPBandH() {
    return BandH;
}

double afGetCrestFactor() {
    return crestFactor;
}

double afGetSpectralFlux() {
    return spectralFlux;
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

uint64_t __MagnitudeIndex = 0;
double __getBeatMagnitude(int beatIndex){
    double sample = magnitudeBeatBuffer[beatIndex][__MagnitudeIndex];
    __MagnitudeIndex++;
    if(__MagnitudeIndex == FFT_N2_LENGTH) { __MagnitudeIndex = 0; }
    return sample; 
}

void __resetIndexDebug(void){
    __audioIndex = 0;
    __envIndex = 0;
    __onsetIndex = 0;
    __TA1Index = 0;
    __TA2Index = 0;
    __MagnitudeIndex = 0;
}