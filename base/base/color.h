#pragma once

#include <cstdint>

namespace Rc
{
    struct Hsva
    {
        double h {0.0f};
        double s {0.0f};
        double v {0.0f};
        double a {1.0f};
    };

    struct Rgba
    {
        uint8_t r {0};
        uint8_t g {0};
        uint8_t b {0};
        uint8_t a {0};
    };

    class Color
    {
    public:
        double r {0.0f};
        double g {0.0f};
        double b {0.0f};
        double a {0.0f};

        constexpr Color(double r, double g, double b, double a = 1.0) :
            r{r}, g{g}, b{b}, a{a}
        {}

        constexpr Color(Rgba const& value) :
            r{value.r / 255.0},
            g{value.g / 255.0},
            b{value.b / 255.0},
            a{value.a / 255.0}
        {}

        // Make RGB color form HSV
        Color(Hsva const& hsv);
        
        // From string...

        Color DecodeSrgba() const;
        Color EncodeSrgba() const;

        operator Hsva() const noexcept;

        operator Rgba() const noexcept
        {
            return
            {
                static_cast<uint8_t>(r * 255.0),
                static_cast<uint8_t>(g * 255.0),
                static_cast<uint8_t>(b * 255.0),
                static_cast<uint8_t>(a * 255.0)
            };
        }

        Color& operator+=(Color const& rhs) noexcept
        {
            r += rhs.r;
            g += rhs.g;
            b += rhs.b;
            a += rhs.a;
            return *this;
        }

        Color operator+(Color const& rhs) const noexcept
        {
            return {r + rhs.r, g + rhs.g, b + rhs.b, a + rhs.a};
        }

        Color& operator-=(Color const& rhs) noexcept
        {
            r -= rhs.r;
            g -= rhs.g;
            b -= rhs.b;
            a -= rhs.a;
            return *this;
        }

        Color operator-(Color const& rhs) const noexcept
        {
            return {r - rhs.r, g - rhs.g, b - rhs.b, a - rhs.a};
        }

        Color operator*(double s) const noexcept
        {
            return {r * s, g * s, b * s, a * s};
        }

        Color operator*=(double s) noexcept
        {
            r *= s;
            g *= s;
            b *= s;
            a *= s;
            return *this;
        }

        Color operator/(double s) const noexcept
        {
            return {r / s, g / s, b / s, a / s};
        }

        Color operator/=(double s) noexcept
        {
            r /= s;
            g /= s;
            b /= s;
            a /= s;
            return *this;
        }
    };

    inline Color operator*(double s, Color const& c) noexcept
    {
        return {c.r * s, c.g * s, c.b * s, c.a * s};
    }

} // Rc