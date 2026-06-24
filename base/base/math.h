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
    constexpr double e = std::numbers::e;
    constexpr double log2e = std::numbers::log2e;
    constexpr double log10e = std::numbers::log10e;
    constexpr double pi = std::numbers::pi;
    constexpr double inv_pi = std::numbers::inv_pi;
    constexpr double inv_sqrtpi = std::numbers::inv_sqrtpi;
    constexpr double ln2 = std::numbers::ln2;
    constexpr double ln10 = std::numbers::ln10;
    constexpr double sqrt2 = std::numbers::sqrt2;
    constexpr double sqrt3 = std::numbers::sqrt3;
    constexpr double inv_sqrt3 = std::numbers::inv_sqrt3;
    constexpr double egamma = std::numbers::egamma;
    constexpr double phi = std::numbers::phi;
    
    constexpr float fe = std::numbers::e_v<float>;
    constexpr float flog2e = std::numbers::log2e_v<float>;
    constexpr float flog10e = std::numbers::log10e_v<float>;
    constexpr float fpi = std::numbers::pi_v<float>;
    constexpr float finv_pi = std::numbers::inv_pi_v<float>;
    constexpr float finv_sqrtpi = std::numbers::inv_sqrtpi_v<float>;
    constexpr float fln2 = std::numbers::ln2_v<float>;
    constexpr float fln10 = std::numbers::ln10_v<float>;
    constexpr float fsqrt2 = std::numbers::sqrt2_v<float>;
    constexpr float fsqrt3 = std::numbers::sqrt3_v<float>;
    constexpr float finv_sqrt3 = std::numbers::inv_sqrt3_v<float>;
    constexpr float fegamma = std::numbers::egamma_v<float>;
    constexpr float fphi = std::numbers::phi_v<float>;

    template <typename T>
    inline bool Range(T value, T min, T max) noexcept
    {
        return (value >= min) && (value <= max);
    }

    constexpr double DegToRad(double degrees)
    {
        return degrees * (Math::pi / 180.0);
    }

    constexpr float DegToRad(float degrees)
    {
        return degrees * (Math::fpi / 180.0f);
    }
    
} // Rc::Math