#pragma once

#include "transformations.h"
#include <cassert>

namespace Rc::Gfx
{
    class Camera
    {
    public:
        //Camera() = default;

        Matrix4<double> GetTransformationMatrix() const
        {
            auto t = transformations;
            t.scale = 1.0;
            return t.GetTransformations();
        }

        void SetFov(double rad)
        {
            assert((rad > 0) && (rad < Math::pi));

            m_fov = rad;
        }

        double Fov() const
        {
            return m_fov;
        }

        Transformations transformations;

    private:
        // Horizontal FOV in radians
        double m_fov {Math::pi / 2.0};
    };

} // Rc::Gfx