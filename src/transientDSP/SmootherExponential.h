#pragma once

#include "./envDeps/constants.h"
#include <memory>

// Exponential smoother (single stage)
class SmootherExponentialSingleStage
{
  public:
    SmootherExponentialSingleStage();
    ~SmootherExponentialSingleStage() = default;

    void reset(double _sample_rate);

    void set_attack(double time_ms, double cm);
    void set_release(double time_ms, double cm);

    double process(double x);

  private:
    double sample_rate;
    double state;

    double alpha_attack;
    double alpha_release;
};

// Exponential smoother (n-stage)
class SmootherExponential
{
  public:
    SmootherExponential();
    ~SmootherExponential() = default;

    void init(int _num_stages);

    void reset(double sample_rate);

    void set_attack(double time_ms, int num_stages = 1);
    void set_release(double time_ms, int num_stages = 1);

    double process(double x);

  private:
    int num_stages;
    double cm;
    std::unique_ptr<SmootherExponentialSingleStage[]> smoothers;
};