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

    Matrix4<double> Transformations::Local() const noexcept
    {
        assert(scale > 0);

        // 1. scale
        // 2. translate pivot
        // 3. rotate

        auto m = Matrix4<double>::Scale(scale, scale, scale);
        m.PrependTranslation(pivot_x, pivot_y, pivot_z);
        m.PrependTransformations(GetRotations().ToMatrix<double>());

        return m;
    }

    Matrix4<double> Transformations::World() const noexcept
    {
        return Matrix4<double>::Translation(x, y, z);
    }

    Matrix4<double> Transformations::GetTransformations() const noexcept
    {
        assert(scale > 0);

        // 1. scale
        // 2. rotate
        // 3. translate

        auto m = Matrix4<double>::Scale(scale, scale, scale);
        m.PrependTransformations(GetRotations().ToMatrix<double>());
        m.PrependTranslation(x, y, z);

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

        m.PrependTransformations(rotations.ToMatrix<double>());

        const auto s = std::lerp(scale, to.scale, ratio);
        m.PrependScaling(s, s, s);

        return m;
    }

} // Rc::Gfx