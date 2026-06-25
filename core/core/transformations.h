#pragma once

#include "base/math.h"

namespace Rc::Gfx
{
    class Transformations
    {
    public:
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

        Quaternion GetRotations() const noexcept;

        Matrix4<double> GetTransformations() const noexcept;

        Matrix4<double> LerpTransformations(Transformations const& to, double ratio) const noexcept;

        // Lock()
        // Unlock()
        // bool Locked()
    };

} // Rc::Gfx