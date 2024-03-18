#pragma once

#include <cmath>

namespace bdsp::constants
{
/**
 * @brief Minimum filter cutoff frequency (approx. lower threshold of human
 * hearing)
 */
constexpr double MIN_FILTER_FREQ = 20.0;

/**
 * @brief Maximum filter cutoff frequency (approx. upper threshold of human
 * hearing)
 */
constexpr double MAX_FILTER_FREQ = 20480.0;

} // namespace bdsp::constants
