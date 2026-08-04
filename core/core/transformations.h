#pragma once

#include "base/math.h"

namespace Rc::Gfx
{
    class Transformations
    {
    public:
        double pivot_x {0};
        double pivot_y {0};
        double pivot_z {0};

        double x {0};
        double y {0};
        double z {0};

        // Y-axys rotation in radians - third applied
        double yaw {0};

        // X-axys rotation in radians - second applied
        double pitch {0};

        // Z-axys rotation in radians - first applied
        double roll {0};

        // Uniform scale; must be greater than 0
        double scale {1};

        Matrix4<double> Local() const noexcept;
        Matrix4<double> World() const noexcept;

        // ...

        Quaternion GetRotations() const noexcept;

        Matrix4<double> GetTransformations() const noexcept;

        // TODO: Static with parameters from, to, ratio
        Matrix4<double> LerpTransformations(Transformations const& to, double ratio) const noexcept;

        // Lock()
        // Unlock()
        // bool Locked()
    };

} // Rc::Gfx