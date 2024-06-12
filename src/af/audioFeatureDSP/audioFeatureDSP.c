#include "audioFeatureDSP.h"
#include "../../hal/globalDefinitions.hpp"

//Spectral Flattness (per frame) -- FREQUENCY DOMAIN
//Spectral Centroid (per frame) -- FREQUENCY DOMAIN
//Spectral flux (per frame) -- FREQUENCY DOMAIN //THIS IS PROPABLY A PER BUFFER THING
//4 BAND EQ -- (per frame) -- FREQUENCY DOMAIN
//Crest Factor (whole buffer) -- TIME DOMAIN

double calculateSpectralCentroid(uint64_t onsetIndex)
{
    double _spectralCentroid = 0.0;
    double magnitudeSum = 0.0;
    for (uint32_t i = 0; i < FFT_N2_LENGTH; i++)
    {
        _spectralCentroid += (magnitudeBeatBuffer[onsetIndex][i] * i);
        magnitudeSum += magnitudeBeatBuffer[onsetIndex][i];
    }
    
    if(magnitudeSum != 0)
        _spectralCentroid /= magnitudeSum;
    
    return (_spectralCentroid / FFT_N2_LENGTH) * sampleRate;
}

double calculateBandL(uint64_t onsetIndex) {
    return (magnitudeBeatBuffer[onsetIndex][1] + magnitudeBeatBuffer[onsetIndex][2] ) / 2.0; //187Hz
}
//BIN * 96000 / 512 = freq
//BIN * 48000 / 512 = freq
double calculateBandML(uint64_t onsetIndex) {
    double ret = 0;
    for(int i = 3; i <= 10; i++)
    {
        ret += (magnitudeBeatBuffer[onsetIndex][i]); //937Hz
    } 
    return (ret / (11-3));
}

double calculateBandMH(uint64_t onsetIndex) {
    double ret = 0;
    for(int i = 11; i <= 22; i++)
    {
        ret += (magnitudeBeatBuffer[onsetIndex][i]); //2 062Hz
    }
    return (ret / (23-11));
}

double calculateBandH(uint64_t onsetIndex) {
    double ret = 0;
    for(int i = 23; i <= 214; i++)
    {
        ret += (magnitudeBeatBuffer[onsetIndex][i]); //20 062Hz
    }
    return (ret / (215-23));
}

double calculateCrestFactor(double* buffer, uint64_t audioIndex, uint64_t length) {
    double peak = 0.0;
    double rms = 0.0;

    // Find the peak value
    for (int i = 0; i < length; i++) {
        if (fabs(buffer[audioIndex + i]) > peak) {
            peak = fabs(buffer[audioIndex + i]);
        }
    }

    // Calculate RMS (Root Mean Square) value
    for (int i = 0; i < length; i++) {
        rms += buffer[audioIndex + i] * buffer[audioIndex + i];
    }
    rms = sqrt(rms / length);

    // Avoid division by zero
    if (rms == 0.0) {
        return 0.0; 
    }

    // Calculate crest factor
    return peak / rms;
}

