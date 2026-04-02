#pragma once

#include <cmath>
#include <complex>
#include <numbers>
#include "vector.h"
#include "matrix.h"
#include "quaternion.h"

// Math constants.
namespace Rc::Math
{
    constexpr auto e = std::numbers::e_v<float>;
    constexpr auto log2e = std::numbers::log2e_v<float>;
    constexpr auto log10e = std::numbers::log10e_v<float>;
    constexpr auto pi = std::numbers::pi_v<float>;
    constexpr auto inv_pi = std::numbers::inv_pi_v<float>;
    constexpr auto inv_sqrtpi = std::numbers::inv_sqrtpi_v<float>;
    constexpr auto ln2 = std::numbers::ln2_v<float>;
    constexpr auto ln10 = std::numbers::ln10_v<float>;
    constexpr auto sqrt2 = std::numbers::sqrt2_v<float>;
    constexpr auto sqrt3 = std::numbers::sqrt3_v<float>;
    constexpr auto inv_sqrt3 = std::numbers::inv_sqrt3_v<float>;
    constexpr auto egamma = std::numbers::egamma_v<float>;
    constexpr auto phi = std::numbers::phi_v<float>;

    template <typename T>
    inline bool Range(T value, T min, T max)
    {
        return (value >= min) && (value <= max);
    }
    
} // Rc::Math