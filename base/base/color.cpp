#include "color.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <utility>

namespace Rc
{
    Color Color::Hsv(float h, float s, float v, float a) noexcept
    {
        assert((h >= 0.0f) && (h <= 1.0f));
        assert((s >= 0.0f) && (s <= 1.0f));
        assert((v >= 0.0f) && (v <= 1.0f));
        assert((a >= 0.0f) && (a <= 1.0f));

        float const hdeg = std::fmod(h * 360.0f, 360.0f);
        int const hi = static_cast<int>(hdeg / 60.0f) % 6;
        float const f = (hdeg / 60.0f) - static_cast<float>(hi);
        float const p = v * (1.0f - s);
        float const q = v * (1.0f - (f * s));
        float const t = v * (1.0f - ((1.0f - f) * s));
        
        switch (hi)
        {
            case 0: return{v, t, p, a};
            case 1: return{q, v, p, a};
            case 2: return{p, v, t, a};
            case 3: return{p, q, v, a};
            case 4: return{t, p, v, a};
            case 5: return{v, p, q, a};
        }

        std::unreachable();
    }

    void Color::Set(float r, float g, float b, float a) noexcept
    {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }

    Color::operator HSV() const noexcept
    {
        float const max = std::max(std::max(r, g), b);
        float const min = std::min(std::min(r, g), b);
        
        // Hue
        float h = 0;
        if (max == min)
        {
            // h = 0;
        }
        else if (max == r)
        {
            if (g >= b)
            {
                h = 60.0f * (g - b) / (max - min);
            }
            else
            {
                h = (60.0f * (g - b) / (max - min)) + 360.0f;
            }
        }
        else if (max == g)
        {
            h = (60.0f * (b - r) / (max - min)) + 120.0f;
        }
        else
        {
            h = (60.0f * (r - g) / (max - min)) + 240.0f;
        }

        // Saturation
        float s = 0.0f;
        if (max != 0)
        {
            s = 1.0f - (min / max);
        }

        return {h / 360.0f, s, max, a};
    }
    
} // Rc
