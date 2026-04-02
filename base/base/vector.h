#pragma once

#include <cmath>
#include <cassert>

namespace Rc
{
    // 4-dimmensional column-major vector
    // w = 1 for points
    // w = 0 for directions
    //
    template<typename T>
    class alignas(16) Vector4
    {
    public:
        T x {0};
        T y {0};
        T z {0};
        T w {0};

        // Assignment.
        constexpr void Set(T x, T y, T z, T w) noexcept
        {
            this->x = x;
            this->y = y;
            this->z = z;
            this->w = w;
        }

        // Assignment (the 'w' is ignored).
        constexpr void Set(T x, T y, T z) noexcept
        {
            this->x = x;
            this->y = y;
            this->z = z;
        }

        // Return normal vector.
        static Vector4 Normal(Vector4 const& a, Vector4 const& b) noexcept
        {
            return Normalized(Cross(a, b));
        }

        // Return normalized vector
        static Vector4 Normalized(Vector4 const& v) noexcept
        {
            auto out = v;
            out.Normalize();
            return out;
        }

        // Set length to 1.
        void Normalize() noexcept
        {
            T const l = Length();
            assert(l != 0);
            Set(x / l, y / l, z / l);
        }

        // Returns orthogonal projection a b the b.
        static Vector4 Projection(Vector4 const& a, Vector4 const& b) noexcept
        {
            return (b * (a * b)) / b.LengthSquare();
        }

        // Return length of the result of the orthogonal projection a b the b.
        static T ProjectionLength(Vector4 const& a, Vector4 const& b) noexcept
        {
            return (a * b) / b.Length();
        }

        // Angle between two vectors in radians.
        static T Angle(Vector4 const& a, Vector4 const& b) noexcept
        {
            return std::acos(CosAngle(a, b));
        }

        // Cosinus of the angle between two vectors.
        static T CosAngle(Vector4 const& a, Vector4 const& b) noexcept
        {
            return (a * b) / (a.Length() * b.Length());
        }

        // Dot product.
        static T Dot(Vector4 const& a, Vector4 const& b) noexcept
        {
            return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
        }

        // Cross product.
        static Vector4 Cross(Vector4 const& l, Vector4 const& r) noexcept
        {
            return {
                (l.y * r.z) - (l.z * r.y),
                (l.z * r.x) - (l.x * r.z),
                (l.x * r.y) - (l.y * r.x),
                0
            };
        }

        // Linear interpolation.
        static Vector4 Lerp(Vector4 const& a, Vector4 const& b, T t)
        {
            return {
                a.x + ((b.x - a.x) * t),
                a.y + ((b.y - a.y) * t),
                a.z + ((b.z - a.z) * t),
                a.w + ((b.w - a.w) * t)
            };
        }

        // Get length.
        T Length() const noexcept
        {
            return std::sqrt((x * x) + (y * y) + (z * z));
        }

        // Get length square.
        T LengthSquare() const noexcept
        {
            return (x * x) + (y * y) + (z * z);
        }

        // Set length. The initial must not be 0.
        void SetLength(T value) noexcept
        {
            T const l = Length();
            assert(l != 0);
            T const f = value / l;
            Set(x * f, y * f, z * f);
        }

        // Vector4 negation.
        void Invert() noexcept
        {
            Set(-x, -y, -z);
        }

        // Dot product.
        T operator*(Vector4 const& r) const noexcept
        {
            return (x * r.x) + (y * r.y) + (z * r.z);
        }

        // Negation
        Vector4 operator-() const noexcept
        {
            return {-x, -y, -z, w};
        }

        Vector4& operator+=(Vector4 const& r) noexcept
        {
            Set(x + r.x, y + r.y, z + r.z, w + r.w);
            return *this;
        }

        Vector4 operator+(Vector4 const& r) const noexcept
        {
            return {x + r.x, y + r.y, z + r.z, w + r.w};
        }

        Vector4& operator-=(Vector4 const& r) noexcept
        {
            Set(x - r.x, y - r.y, z - r.z, w - r.w);
            return *this;
        }

        Vector4 operator-(Vector4 const& r) const noexcept
        {
            return {x - r.x, y - r.y, z - r.z, w - r.w};
        }

        Vector4& operator*=(T value) noexcept
        {
            Set(x * value, y * value, z * value, w * value);
            return *this;
        }

        Vector4 operator*(T value) const noexcept
        {
            return {x * value, y * value, z * value, w * value};
        }

        Vector4& operator/=(T value) noexcept
        {
            assert(value != 0);
            Set(x / value, y / value, z / value, w / value);
            return *this;
        }

        Vector4 operator/(T value) const noexcept
        {
            assert(value != 0);
            return {x / value, y / value, z / value, w / value};
        }

        bool operator>(Vector4 const& r) const noexcept
        {
            return LengthSquare() > r.LengthSquare();
        }

        bool operator>=(Vector4 const& r) const noexcept
        {
            return LengthSquare() >= r.LengthSquare();
        }

        bool operator<(Vector4 const& r) const noexcept
        {
            return LengthSquare() < r.LengthSquare();
        }

        bool operator<=(Vector4 const& r) const noexcept
        {
            return LengthSquare() <= r.LengthSquare();
        }
    };

} // Rc

template<typename T>
static inline Rc::Vector4<T> operator*(float value, Rc::Vector4<T> const& vector)
{
    return vector * value;
}
