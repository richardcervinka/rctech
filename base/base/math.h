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
    
    constexpr double Sin(double x)
    {
        return std::sin(x);
    }

    constexpr double Cos(double x)
    {
        return std::cos(x);
    }

    constexpr double Tan(double x)
    {
        return std::tan(x);
    }

    template <typename T>
    constexpr bool InRange(T value, T min, T max) noexcept
    {
        return (value >= min) && (value <= max);
    }

    constexpr double DegToRad(double degrees)
    {
        return degrees * (Math::pi / 180.0);
    }

    constexpr double RadToDeg(double radians)
    {
        return radians * (180.0 / Math::pi);
    }

    // Sinus Cardinalis
    constexpr double Sinc(double x)
    {
        if (x == 0)
        {
            return 1;
        }
        return std::sin(x) / x;
    }

    // Normalized Sinus Cardinalis
    constexpr double SincNorm(double x)
    {
        if (x == 0)
        {
            return 1;
        }
        return std::sin(x * pi) / (x * pi);
    }

    constexpr double Log2(double x)
    {
        return std::log2(x);
    }

    constexpr double Log(double x)
    {
        return std::log(x);
    }

    constexpr double Pow(double base, double x)
    {
        return std::pow(base, x);
    }

    constexpr uint64_t Exp2(uint64_t x)
    {
        return uint64_t{1} << x;
    }

    constexpr uint32_t Exp2(uint32_t x)
    {
        return uint32_t{1} << x;
    }

    constexpr double Exp2(double x)
    {
        return std::exp2(x);
    }
    
} // Rc::Math