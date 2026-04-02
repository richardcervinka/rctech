#include "quaternion.h"

namespace Rc
{
    Quaternion Quaternion::Slerp(Quaternion const& a, Quaternion b, float t) noexcept
    {
        assert((t >= 0) && (t <= 1));

        auto ab = Quaternion::Dot(a, b);

        // Select a shorter path.
        if (ab < 0)
        {
            ab = -ab;
            b = -b;
        }

        // Linear interpolation fallback.
        if (ab > 0.9995f)
        {
            return Quaternion::Normalized(a + (t * (b - a)));
        }

        // Quaternions slerp.

        auto const theta = std::acosf(ab);
        auto const sint = std::sqrtf(1.f - (ab * ab));
        auto const j = std::sinf(theta * (1.f - t)) / sint;
        auto const k = std::sinf(theta * t) / sint;

        return (a * j) + (b * k);
    }

} // Rc