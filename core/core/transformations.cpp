#include "transformations.h"
#include <cassert>

namespace Rc::Gfx
{
    Quaternion Transformations::GetRotations() const noexcept
    {
        Quaternion q = Quaternion::RotationY(static_cast<float>(yaw));
        q = q * Quaternion::RotationX(static_cast<float>(pitch));
        q = q * Quaternion::RotationZ(static_cast<float>(roll));
        q.Normalize();

        return q;
    }

    Matrix4<double> Transformations::GetTransformations() const noexcept
    {
        assert(scale > 0);

        // 1. scale
        // 2. rotate
        // 3. translate

        auto m = Matrix4<double>::Translation(x, y, z);
        m.AppendTransformations(GetRotations().ToMatrix<double>());
        m.AppendScaling(scale, scale, scale);

        return m;
    }

    Matrix4<double> Transformations::LerpTransformations(Transformations const& to, double ratio) const noexcept
    {
        assert(scale > 0);
        assert(to.scale > 0);

        // 1. scale
        // 2. rotate
        // 3. translate

        const auto rotations = Quaternion::Slerp(GetRotations(), to.GetRotations(), static_cast<float>(ratio));

        auto m = Matrix4<double>::Translation(
            std::lerp(x, to.x, ratio),
            std::lerp(y, to.y, ratio),
            std::lerp(z, to.z, ratio)
        );

        m.AppendTransformations(rotations.ToMatrix<double>());

        const auto s = std::lerp(scale, to.scale, ratio);
        m.AppendScaling(s, s, s);

        return m;
    }

} // Rc::Gfx