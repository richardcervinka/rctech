#include "color.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <utility>

namespace Rc
{
    Color::Color(Hsva const& hsv)
    {
        assert((hsv.h >= 0.0f) && (hsv.h <= 1.0f));
        assert((hsv.s >= 0.0f) && (hsv.s <= 1.0f));
        assert((hsv.v >= 0.0f) && (hsv.v <= 1.0f));
        assert((hsv.a >= 0.0f) && (hsv.a <= 1.0f));

        double const hdeg = std::fmod(hsv.h * 360.0f, 360.0f);
        int const hi = static_cast<int>(hdeg / 60.0f) % 6;
        double const f = (hdeg / 60.0f) - static_cast<double>(hi);
        double const p = hsv.v * (1.0f - hsv.s);
        double const q = hsv.v * (1.0f - (f * hsv.s));
        double const t = hsv.v * (1.0f - ((1.0f - f) * hsv.s));
        
        switch (hi)
        {
            case 0: *this = Color(hsv.v, t, p, hsv.a);
            case 1: *this = Color(q, hsv.v, p, hsv.a);
            case 2: *this = Color(p, hsv.v, t, hsv.a);
            case 3: *this = Color(p, q, hsv.v, hsv.a);
            case 4: *this = Color(t, p, hsv.v, hsv.a);
            case 5: *this = Color(hsv.v, p, q, hsv.a);
        }

        std::unreachable();
    }

    Color::operator Hsva() const noexcept
    {
        double const max = std::max(std::max(r, g), b);
        double const min = std::min(std::min(r, g), b);
        
        // Hue
        double h = 0;
        if (max == min)
        {
            // h = 0;
        }
        else if (max == r)
        {
            if (g >= b)
            {
                h = 60.0 * (g - b) / (max - min);
            }
            else
            {
                h = (60.0 * (g - b) / (max - min)) + 360.0;
            }
        }
        else if (max == g)
        {
            h = (60.0 * (b - r) / (max - min)) + 120.0;
        }
        else
        {
            h = (60.0 * (r - g) / (max - min)) + 240.0;
        }

        // Saturation
        double s = 0.0;
        if (max != 0)
        {
            s = 1.0 - (min / max);
        }

        return {h / 360.0, s, max, a};
    }

    Color Color::DecodeSrgba() const
    {
        static const auto decode = [](double value)
        {
            if (value <= 0.04045)
            {
                return value / 12.92;
            }
            return std::pow((value + 0.055) / 1.055, 2.4);
        };

        return {decode(r), decode(g), decode(b), a};
    }

    Color Color::EncodeSrgba() const
    {
        static const auto encode = [](double value)
        {
            if (value <= 0.0031308)
            {
                return value * 12.92;
            }
            return (1.055 * std::pow(value, 1.0 / 2.4)) - 0.055;
        };

        return {encode(r), encode(g), encode(b), a};
    }

} // Rc
