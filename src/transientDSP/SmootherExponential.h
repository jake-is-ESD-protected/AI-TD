#pragma once

#include "./envDeps/constants.h"
#include <memory>

// Exponential smoother (single stage)
class SmootherExponentialSingleStage
{
  public:
    SmootherExponentialSingleStage();
    ~SmootherExponentialSingleStage() = default;

    void reset(float _sample_rate);

    void set_attack(float time_ms, float cm);
    void set_release(float time_ms, float cm);

    float process(float x);

  private:
    float sample_rate;
    float state;

    float alpha_attack;
    float alpha_release;
};

// Exponential smoother (n-stage)
class SmootherExponential
{
  public:
    SmootherExponential();
    ~SmootherExponential() = default;

    void init(int _num_stages);

    void reset(float sample_rate);

    void set_attack(float time_ms, int num_stages = 1);
    void set_release(float time_ms, int num_stages = 1);

    float get_attack();
    float get_release();

    float process(float x);

  private:
    float attack_time;
    float release_time;

    int num_stages;
    float cm;
    std::unique_ptr<SmootherExponentialSingleStage[]> smoothers;
};