/**
 * @file af.h
 * @author jake-is-ESD-protected
 * @date 2024-03-08
 * @brief Target device audio feature extraction algorithms for AI inputs.
 *
 *
 */

#include "af.h"
#include "transientDetectionDSP/Smoother4ExponentialAF.h"
#include "transientDetectionDSP/EnvelopeFollowerPeakHoldAF.h"
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

bool calculateAFFlag = false; //THIS FLAG IS UP WHEN THE BUFFER IS FULLY RECORDED AND CALCULATIONS COULD START TO HAPPEN
bool calculationsDoneFlag = false; //THIS FLAG IS UP WHEN THE PREPROCESSING IS DONE

BTT *btt;

__attribute__((section(".sdram_bss"))) float audioBuffer[AUDIO_BUFFER_SIZE];
__attribute__((section(".sdram_bss"))) float envBuffer[AUDIO_BUFFER_SIZE];

__attribute__((section(".sdram_bss"))) uint64_t onsetBuffer[MAX_ONSETS];

__attribute__((section(".sdram_bss"))) float onsetT1ABuffer[MAX_ONSETS];
__attribute__((section(".sdram_bss"))) float onsetT2ABuffer[MAX_ONSETS];

__attribute__((section(".sdram_bss"))) float magnitudeBuffer[FFT_N2_LENGTH];

__attribute__((section(".sdram_bss"))) float spectralFluxBuffer[MAX_ONSETS];
__attribute__((section(".sdram_bss"))) float spectralCentroidBuffer[MAX_ONSETS];
__attribute__((section(".sdram_bss"))) float bandLBuffer[MAX_ONSETS];
__attribute__((section(".sdram_bss"))) float bandMLBuffer[MAX_ONSETS];
__attribute__((section(".sdram_bss"))) float bandMHBuffer[MAX_ONSETS];
__attribute__((section(".sdram_bss"))) float bandHBuffer[MAX_ONSETS];
__attribute__((section(".sdram_bss"))) float crestFactorBuffer[MAX_ONSETS];

__attribute__((section(".sdram_bss"))) float magnitudeBeatBuffer[MAX_ONSETS][FFT_N2_LENGTH];

uint64_t audioBufferIndex = 0;
uint64_t audioBufferRuntimeIndex = 0;
uint64_t onsetBufferIndex = 0;
uint64_t spectralFluxIndex = 0;
dft_sample_t dftBuffer[BEAT_DETECTION_BUFFER_SIZE];

bool firstSepctrumFlag = true;

float spectralCentroid = 0;
float BandL = 0; //150Hz
float BandML = 0; //800Hz
float BandMH = 0; //2kHz
float BandH = 0; //20kHz
float crestFactor = 0;
float spectralFlux = 0;
float T1A = 0;
float T2A = 0;

//FILTER START
float filterBuffer;
float a0, b1;
float processFilter(float in)
{
    return filterBuffer = in * a0 + filterBuffer * b1;
}
void setTime(float freq)
{
    b1 = exp(-2.0 * PI * freq/slowsampleRate);
    a0 = 1.0 - b1;
}
//FILTER END

void resetBuffer()
{
    memset(audioBuffer,0, sizeof(float) * AUDIO_BUFFER_SIZE);
    memset(envBuffer,0, sizeof(float) * AUDIO_BUFFER_SIZE);
    memset(onsetBuffer,0, sizeof(uint64_t) * MAX_ONSETS);
    memset(onsetT1ABuffer,0, sizeof(float) * MAX_ONSETS);
    memset(onsetT2ABuffer,0, sizeof(float) * MAX_ONSETS);
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
    EnvFollowerInitAf();
    setTime(slowsampleRate/2);
}

void EnvFollowerInitAf()
{
    EnvelopeFollowerPeakHoldInitAf(1);
    initAll4Af();
    setAttackAll4Af(ENV_SMOOTH_ATTACK);
    setReleaseAll4Af(ENV_SMOOTH_RELEASE);
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
                    slowsampleRate,
                    BTT_DEFAULT_ANALYSIS_LATENCY_ONSET_ADJUSTMENT,
                    BTT_DEFAULT_ANALYSIS_LATENCY_BEAT_ADJUSTMENT
                    );
    // clang-format on
    btt_set_tracking_mode(btt, BTT_ONSET_AND_TEMPO_TRACKING);
    btt_set_onset_tracking_callback(btt, onset_detected_callback, NULL);
}

bool downSamplingFlipFlop = true;

void AFInCAppend(float in)
{
    if (audioBufferIndex < AUDIO_BUFFER_SIZE)
    {
        if(audioBufferIndex < AUDIO_BUFF_FADE_TIME_S)
        {
            audioBuffer[audioBufferIndex] = in * audioBufferIndex / (float) AUDIO_BUFF_FADE_TIME_S; //FADE IN
        }
        else
        {
            audioBuffer[audioBufferIndex] = in;
        }
        audioBufferIndex++;
    }
}

void processBTT()
{
    if(audioBufferRuntimeIndex < audioBufferIndex) //IF THERE ARE NEW SAMPLES TO PROCESS
    {
        dftBuffer[0] = audioBuffer[audioBufferRuntimeIndex];
        btt_process(btt, dftBuffer, 1);
        audioBufferRuntimeIndex++;
    }
    if(calculateAFFlag && audioBufferRuntimeIndex >= audioBufferIndex) //IF WHOLE BUFFER IS PROCESSED AND BTN RELEASED
    {
        AFInCProcess();
        calculateAFFlag = false;
        calculationsDoneFlag = true; //TELL THE MAIN LOOP THAT THE PREPROCESSING IS DONE
    }
    /*
    float tempVar = processFilter(audioBuffer[audioBufferRuntimeIndex]);
    if(downSamplingFlipFlop)
    {
        dftBuffer[0] = tempVar;
        btt_process(btt, dftBuffer, 1);
        downSamplingFlipFlop = false;
    }
    else
    {
        downSamplingFlipFlop = true;
    }
    audioBufferRuntimeIndex++;*/
}

void processBTTAndAFInC() //FUNCTION FOR PROPER USAGE WITH PYTHON
{
    while(audioBufferRuntimeIndex < audioBufferIndex) //IF THERE ARE NEW SAMPLES TO PROCESS
    {
        dftBuffer[0] = audioBuffer[audioBufferRuntimeIndex];
        btt_process(btt, dftBuffer, 1);
        audioBufferRuntimeIndex++;
    }
    AFInCProcess();
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
    for (uint64_t i = 0; i < audioBufferIndex; i++)
    {
        envBuffer[i] = processEnvelopeAf(EnvelopeFollowerPeakHoldProcessAf(audioBuffer[i]));
    }


    for (uint64_t i = 0; i < onsetBufferIndex-1; i++) //COMPENSATE ALL ONSETS FIRST
    {
        if(onsetBuffer[i] > ONSET_DETECTION_COMPENSATION_N)
        {
            onsetBuffer[i] -= ONSET_DETECTION_COMPENSATION_N;
        }
    }

    float magBufferMax = 0;
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

    float magBufferNormalizationFactor = 1.0f / magBufferMax;
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
        bandLBuffer[i] = calculateBandL(i);
        bandMLBuffer[i] = calculateBandML(i);
        bandMHBuffer[i] = calculateBandMH(i);
        bandHBuffer[i] = calculateBandH(i);
        crestFactorBuffer[i] = calculateCrestFactor(audioBuffer, onsetBuffer[i], onsetBuffer[i+1] - onsetBuffer[i]);

    }
    spectralCentroid = findPercentile(spectralCentroidBuffer, onsetBufferIndex, 75); 
    BandL = findPercentile(bandLBuffer, onsetBufferIndex, 75);
    BandML = findPercentile(bandMLBuffer, onsetBufferIndex, 75);
    BandMH = findPercentile(bandMHBuffer, onsetBufferIndex, 75);
    BandH = findPercentile(bandHBuffer, onsetBufferIndex, 75);
    crestFactor = findPercentile(crestFactorBuffer, onsetBufferIndex, 75);
    spectralFlux = findPercentile(spectralFluxBuffer, spectralFluxIndex, 75);
    T1A = findPercentile(onsetT1ABuffer, onsetBufferIndex, 75);
    T2A = findPercentile(onsetT2ABuffer, onsetBufferIndex, 75);
}


uint64_t __afGetIdxOfMax(float *sig, uint64_t fromIdx, uint64_t toIdx)
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

uint64_t __afGetIdxOfMin(float *sig, uint64_t fromIdx, uint64_t toIdx)
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
float afGetT1A() {
    if(T1A > MAX_TA1_SAMPLES) { T1A = MAX_TA1_SAMPLES; }
    return T1A / MAX_TA1_SAMPLES;
}

float afGetT2A() {
    if(T2A > MAX_TA2_SAMPLES) { T2A = MAX_TA2_SAMPLES; }
    return T2A / MAX_TA2_SAMPLES;
}

float afGetSpectralCentroid()
{
    if(spectralCentroid > MAX_CENTROID) { spectralCentroid = MAX_CENTROID; }
    return spectralCentroid / MAX_CENTROID;
}

float afGetTempo() {
    float tempo = btt_get_tempo_bpm(btt);
    if(tempo > MAX_TEMPO){ tempo /= 2.0; } 
    return tempo / MAX_TEMPO;
}

float afGetPBandL() {
    return BandL;
}

float afGetPBandML() {
    return BandML;
}

float afGetPBandMH() {
    return BandMH;
}

float afGetPBandH() {
    return BandH;
}

float afGetCrestFactor() {
    if(crestFactor > MAX_CREST) { crestFactor = MAX_CREST; }
    return crestFactor / MAX_CREST;
}

float afGetSpectralFlux() {
    if(spectralFlux > MAX_FLUX) { spectralFlux = MAX_FLUX; }
    return spectralFlux / MAX_FLUX;
}


// --------------------------------------------------------------------------
// helper functions for dev debugging
// --------------------------------------------------------------------------

uint64_t __audioIndex = 0;
float __getAudioBuffer(void){
    float sample = audioBuffer[__audioIndex];
    __audioIndex++;
    if(__audioIndex == AUDIO_BUFFER_SIZE) { __audioIndex = 0; }
    return sample; 
}

uint64_t __envIndex = 0;
float __getEnvBuffer(void){
    float sample = envBuffer[__envIndex];
    __envIndex++;
    if(__envIndex == AUDIO_BUFFER_SIZE) { __envIndex = 0; }
    return sample; 
}

uint64_t __onsetIndex = 0;
uint64_t __getOnsetBuffer(void){
    float sample = onsetBuffer[__onsetIndex];
    __onsetIndex++;
    if(__onsetIndex == MAX_ONSETS) { __onsetIndex = 0; }
    return sample; 
}

uint64_t __TA1Index = 0;
uint64_t __getTA1Buffer(void){
    float sample = onsetT1ABuffer[__TA1Index];
    __TA1Index++;
    if(__TA1Index == MAX_ONSETS) { __TA1Index = 0; }
    return sample; 
}

uint64_t __TA2Index = 0;
uint64_t __getTA2Buffer(void){
    float sample = onsetT2ABuffer[__TA2Index];
    __TA2Index++;
    if(__TA2Index == MAX_ONSETS) { __TA2Index = 0; }
    return sample; 
}

uint64_t __MagnitudeIndex = 0;
float __getBeatMagnitude(int beatIndex){
    float sample = magnitudeBeatBuffer[beatIndex][__MagnitudeIndex];
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