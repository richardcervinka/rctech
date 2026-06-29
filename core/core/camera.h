#pragma once

#include "transformations.h"
#include <cassert>

namespace Rc::Gfx
{
    class PerspectiveCamera
    {
    public:
        static constexpr double min_fov = 0;
        static constexpr double max_fov = Math::pi;
        
        Matrix4<double> GetProjectionMatrix(int viewport_width, int viewport_height) const;

        // The scale is ignored.
        Transformations transformations;

        // Horizontal FOV in radians
        // Must be > min_fov
        // Must be < max_fov
        double fov {Math::pi / 2.0};

        double z_near {0.01};
        double z_far {1000};
    };

} // Rc::Gfx