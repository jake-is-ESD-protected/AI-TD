/**
 * @file af.h
 * @author jake-is-ESD-protected
 * @date 2024-03-08
 * @brief Target device audio feature extraction algorithms for AI inputs.
 *
 * 
 */

#include "af.h"
#include "transientDSP.hpp"

static SmootherExponential expSmooth;
double lastVarGainValue = 0;


void __afGetEnvelope(double* sig, double* env, uint32_t len)
{
    expSmooth.init(ENV_SMOOTH_ORDER);
    expSmooth.reset(SAMPLE_RATE);
    expSmooth.set_attack(ENV_SMOOTH_ATTACK);
    expSmooth.set_release(ENV_SMOOTH_RELEASE);

    for(uint32_t i = 0; i < len; i++)
    {
        env[i] = expSmooth.process(sig[i]);
    }
}


uint32_t __afGetIdxOfMax(double* sig, uint32_t len, uint32_t fromIdx, uint32_t toIdx)
{
    uint32_t idxMax = fromIdx;
    for(uint32_t i = fromIdx; i < toIdx; i++)
    {
        if(sig[i] > sig[idxMax])
        {
            idxMax = i;
        }
    }
    return idxMax;
}


uint32_t __afGetIdxOfMin(double* sig, uint32_t len, uint32_t fromIdx, uint32_t toIdx)
{
    uint32_t idxMin = fromIdx;
    for(uint32_t i = fromIdx; i < toIdx; i++)
    {
        if(sig[i] < sig[idxMin])
        {
            idxMin = i;
        }
    }
    return idxMin;
}


void afGetTA(double* sig, uint32_t len, uint32_t searchInterval, double* res)
{
    uint32_t nFrames = len / (FRAME_LEN * SAMPLE_RATE);
    double T1A = 0;
    double T2A = 0;
    for(uint32_t i = 0; i < nFrames; i++)
    {
        uint32_t lBound = i * SAMPLE_RATE;
        uint32_t uBound = (i + 1) * SAMPLE_RATE;
        uint32_t idxMax = __afGetIdxOfMax(sig, len, lBound, uBound);
        
        uint32_t start = 0;
        uint32_t stop = 0;
        if(idxMax < searchInterval)
        {
            start = lBound;
        }
        else
        {
            start = idxMax - searchInterval;
        }
        uint32_t idxMinPre = __afGetIdxOfMin(sig, len, start, idxMax);

        if((idxMax + searchInterval) > uBound)
        {
            stop = uBound;
        }
        else
        {
            stop = idxMax + searchInterval;
        }
        uint32_t idxMinPost = __afGetIdxOfMin(sig, len, idxMax, stop);
        T1A += ((idxMax - idxMinPre) / SAMPLE_RATE);
        T2A += ((idxMinPost - idxMax) / SAMPLE_RATE);
    }
    // god help the user to define `res` with a memory of 2 double...
    res[0] = T1A / nFrames;
    res[1] = T2A / nFrames;
}