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
    _spectralCentroid /= magnitudeSum;
    _spectralCentroid += (_spectralCentroid / FFT_N2_LENGTH) * sampleRate;
    
    return _spectralCentroid;
}

double calculateSpectralFlatness(uint64_t onsetIndex)
{
    double spectralflatness = 0; //CHANGED
    double geometricMean = 1.0;
    double arithmeticMean = 0.0;

    for (uint32_t i = 0; i < FFT_N2_LENGTH; i++) {
        if(magnitudeBeatBuffer[onsetIndex][i] != 0.0)
            geometricMean = geometricMean * (double) (magnitudeBeatBuffer[onsetIndex][i]);
        arithmeticMean += magnitudeBeatBuffer[onsetIndex][i];
    }
    if (fabs(arithmeticMean) < 0.00001) {
        return 0; //TODO: WHAT SHOULD THE NEUTRAL ELEMENT BE?
    }

    geometricMean = pow(geometricMean, 1.0 / (double) FFT_N2_LENGTH);
    
    arithmeticMean /= (double) FFT_N2_LENGTH;
    
    spectralflatness += (geometricMean / arithmeticMean);
    return spectralflatness;
}

double calculateBandL(uint64_t onsetIndex) {
    //do some shit with  magnitudeBeatBuffer
    return 10;
}

double calculateBandML(uint64_t onsetIndex) {
    return 1;
}

double calculateBandMH(uint64_t onsetIndex) {
    return 1;
}

double calculateBandH(uint64_t onsetIndex) {
    return 1;
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
        rms += buffer[audioIndex + i] * buffer[i];
    }
    rms = sqrt(rms / length);

    // Avoid division by zero
    if (rms == 0.0) {
        return 0.0; 
    }

    // Calculate crest factor
    return peak / rms;
}

