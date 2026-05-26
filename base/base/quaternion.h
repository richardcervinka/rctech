#pragma once

#include <cmath>
#include <cassert>
#include "vector.h"
#include "matrix.h"

namespace Rc
{
    class alignas(16) Quaternion
    {
    public:
        float w {0};
        float x {0};
        float y {0};
        float z {0};

        // Assignment.
        constexpr void Set(float w, float x, float y, float z) noexcept
        {
            this->w = w;
            this->x = x;
            this->y = y;
            this->z = z;
        }

        // Get length.
        float Length() const noexcept
        {
            return std::sqrt((w * w) + (x * x) + (y * y) + (z * z));
        }

        // Get length square.
        float LengthSquare() const noexcept
        {
            return (w * w) + (x * x) + (y * y) + (z * z);
        }

        // Set length to 1.
        void Normalize() noexcept
        {
            float const l = Length();
            assert(l != 0);
            Set(w / l, x / l, y / l, z / l);
        }

        
        // Return normalized quaternion
        static Quaternion Normalized(Quaternion const& q) noexcept
        {
            auto out = q;
            out.Normalize();
            return out;
        }

        // Conjugation is equal to inversion when the quaternion is normalized.
        void Conjugate() noexcept
        {
            Set(w, -x, -y, -z);
        }

        static Quaternion Conjugated(Quaternion const& q) noexcept
        {
            return {q.w, -q.x, -q.y, -q.z};
        }

        // Use Conjugate when quaternion is normalized.
        void Invert() noexcept
        {
            float const q = LengthSquare();
            assert(q != 0);

            Set(w / q, -x / q, -y / q, -z / q);
        }

        static Quaternion Inversed(Quaternion q) noexcept
        {
            q.Invert();
            return q;
        }

        // Dot product.
        static float Dot(Quaternion const& a, Quaternion const& b) noexcept
        {
            return (a.w * b.w) + (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
        }

        Quaternion operator*(Quaternion const& r) const noexcept
        {
            return {
                (w * r.w) - (x * r.x) - (y * r.y) - (z * r.z),
                (w * r.x) + (x * r.w) + (y * r.z) - (z * r.y),
                (w * r.y) - (x * r.z) + (y * r.w) + (z * r.x),
                (w * r.z) + (x * r.y) - (y * r.x) + (z * r.w)
            };
        }

        // Create rotation quaternion around global axis x.
        static Quaternion RotationX(float rad) noexcept
        {
            return {std::cosf(rad / 2.f), std::sinf(rad / 2.f), 0, 0};
        }

        // Create rotation quaternion around global axis y.
        static Quaternion RotationY(float rad) noexcept
        {
            return {std::cosf(rad / 2.f), 0, std::sinf(rad / 2.f), 0};
        }

        // Create rotation quaternion around global axis z.
        static Quaternion RotationZ(float rad) noexcept
        {
            return {std::cosf(rad / 2.f), 0, 0, std::sinf(rad / 2.f)};
        }

        // Create rotation quaternion around the normalized axis 'v'
        template<typename T>
        static Quaternion Rotation(float rad, Vector4<T> const& v) noexcept
        {
            auto const c = std::cosf(rad / 2.f);
            auto const s = std::sinf(rad / 2.f);
            return {c, v.x * s, v.y * s, v.z * s};
        }

        static Quaternion Slerp(Quaternion const& a, Quaternion b, float t) noexcept;

        template<typename T>
        Matrix4<T> ToMatrix() const noexcept
        {
            auto const xx = x * x * 2;
            auto const yy = y * y * 2;
            auto const zz = z * z * 2;
            auto const xy = x * y * 2;
            auto const xz = x * z * 2;
            auto const yz = y * z * 2;
            auto const wx = w * x * 2;
            auto const wy = w * y * 2;
            auto const wz = w * z * 2;

            return {
                1 - yy - zz,    xy - wz,        xz + wy,        0,
                xy + wz,        1 - xx - zz,    yz - wx,        0,
                xz - wy,        yz + wx,        1 - xx - yy,    0,
                0,              0,              0,              1
            };
        }

        // Negation
        Quaternion operator-() const noexcept
        {
            return {-w, -x, -y, -z};
        }

        Quaternion& operator+=(Quaternion const& r) noexcept
        {
            Set(w + r.w, x + r.x, y + r.y, z + r.z);
            return *this;
        }

        Quaternion operator+(Quaternion const& r) const noexcept
        {
            return {w + r.w, x + r.x, y + r.y, z + r.z};
        }

        Quaternion& operator-=(Quaternion const& r) noexcept
        {
            Set(w - r.w, x - r.x, y - r.y, z - r.z);
            return *this;
        }

        Quaternion operator-(Quaternion const& r) const noexcept
        {
            return {w - r.w, x - r.x, y - r.y, z - r.z};
        }

        Quaternion& operator*=(float value) noexcept
        {
            Set(w * value, x * value, y * value, z * value);
            return *this;
        }

        Quaternion operator*(float value) const noexcept
        {
            return {w * value, x * value, y * value, z * value};
        }

        Quaternion& operator/=(float value) noexcept
        {
            assert(value != 0);
            Set(w / value, x / value, y / value, z / value);
            return *this;
        }

        Quaternion operator/(float value) const noexcept
        {
            assert(value != 0);
            return {w / value, x / value, y / value, z / value};
        }
    };

} // Rc

static inline Rc::Quaternion operator*(float value, Rc::Quaternion const& q)
{
    return q * value;
}
