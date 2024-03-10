#pragma once

constexpr double PI = 3.14159265358979323846264338327950288419716939937510582097494459230781640628620899;
constexpr double TWO_PI = 2.0 * PI;
constexpr double MIN_FILTER_FREQ = 20.0;
constexpr double MAX_FILTER_FREQ = 20480.0;

#include <memory>

// Exponential smoother (single stage)
class SmootherExponentialSingleStage
{
  public:
    SmootherExponentialSingleStage();
    ~SmootherExponentialSingleStage() = default;

    void reset(double _sample_rate);

    void set_attack(double time_ms, int num_stages = 1);
    void set_release(double time_ms, int num_stages = 1);

    double process(double x);

  private:
    double sample_rate;
    double state;

    double alpha_attack;
    double alpha_release;

    static double cutoff_multiplier(int num_stages);
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
    std::unique_ptr<SmootherExponentialSingleStage[]> smoothers;
};