#pragma once

#include "base/math.h"

namespace Rc
{
    class Transformations
    {
    public:
        void SetYaw(float value) { m_yaw = value; }
        float GetYaw() const { return m_yaw; }
        void SetPitch(float value) { m_pitch = value; }
        float GetPitch() const { return m_pitch; }
        void SetRoll(float value) { m_roll = value; }
        float GetRoll() const { return m_roll; }

        Matrix4<float> GetTransformations() // double ?
        {
            Matrix4<float> m;

            // ...

            return m;
        }

    private:
        float m_x {0};
        float m_y {0};
        float m_z {0};

        float m_yaw {0};
        float m_pitch {0};
        float m_roll {0};

        Quaternion m_yaw_q;
        Quaternion m_pitch_q;
        Quaternion m_roll_q;
    };

} // Rc