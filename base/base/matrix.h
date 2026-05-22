#pragma once

#include <cstring>
#include <stdexcept>
#include <optional>
#include "vector.h"

namespace Rc
{
    // 4x4 matrix column-major.
    // Right-Handed.
    template<typename T>
    class alignas(16) Matrix4
    {
    public:
        static constexpr std::size_t rows = 4;
        static constexpr std::size_t cols = 4;

        // Create a zero matrix.
        Matrix4() = default;

        // Create matrix with all elements initialized to the value.
        explicit Matrix4(T value)
        {
            std::fill_n(std::begin(m[0]), 16, value);
        }

        constexpr Matrix4(
            T r0_c0, T r0_c1, T r0_c2, T r0_c3,
            T r1_c0, T r1_c1, T r1_c2, T r1_c3,
            T r2_c0, T r2_c1, T r2_c2, T r2_c3,
            T r3_c0, T r3_c1, T r3_c2, T r3_c3) :
                m {
                    {r0_c0, r1_c0, r2_c0, r3_c0},
                    {r0_c1, r1_c1, r2_c1, r3_c1},
                    {r0_c2, r1_c2, r2_c2, r3_c2},
                    {r0_c3, r1_c3, r2_c3, r3_c3}
                }
        {}

        // Typecast
        template<typename U>
        Matrix4<U> To() const noexcept
        {
            return {
                static_cast<U>(At(0, 0)),
                static_cast<U>(At(0, 1)),
                static_cast<U>(At(0, 2)),
                static_cast<U>(At(0, 3)),
                static_cast<U>(At(1, 0)),
                static_cast<U>(At(1, 1)),
                static_cast<U>(At(1, 2)),
                static_cast<U>(At(1, 3)),
                static_cast<U>(At(2, 0)),
                static_cast<U>(At(2, 1)),
                static_cast<U>(At(2, 2)),
                static_cast<U>(At(2, 3)),
                static_cast<U>(At(3, 0)),
                static_cast<U>(At(3, 1)),
                static_cast<U>(At(3, 2)),
                static_cast<U>(At(3, 3))
            };
        }

        // Create an identity matrix.
        static Matrix4 Identity() noexcept
        {
            return {
                1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1
            };
        }

        // Create a zero matrix.
        static Matrix4 Zero() noexcept
        {
            return {};
        }

        // Create transpose matrix.
        static Matrix4 Transpose(Matrix4 const& m) noexcept
        {
            return {
                m.At(0, 0), m.At(1, 0), m.At(2, 0), m.At(3, 0),
                m.At(0, 1), m.At(1, 1), m.At(2, 1), m.At(3, 1),
                m.At(0, 2), m.At(1, 2), m.At(2, 2), m.At(3, 2),
                m.At(0, 3), m.At(1, 3), m.At(2, 3), m.At(3, 3)
            };
        }

        // Create inverse matrix.
        static std::optional<Matrix4> Inverse(Matrix4 const& m) noexcept
        {
            Matrix4 inverted;
            if (!Matrix4::Invert(m, inverted))
            {
                return std::nullopt;
            }
            return inverted;
        }

        // Create rotation matrices around the world axe.
        static Matrix4 RotationX(T rad) noexcept
        {
            T const sin = std::sin(rad);
            T const cos = std::cos(rad);

            return {
                1,    0,    0,    0,
                0,    cos, -sin,  0,
                0,    sin,  cos,  0,
                0,    0,    0,    1
            };
        }

        // Create rotation matrices around the world axe.
        static Matrix4 RotationY(T rad) noexcept
        {
            T const sin = std::sin(rad);
            T const cos = std::cos(rad);

            return {
                cos,  0,    sin,  0,
                0,    1,    0,    0,
                -sin, 0,    cos,  0,
                0,    0,    0,    1
            };
        }

        // Create rotation matrices around the world axe.
        static Matrix4 RotationZ(T rad) noexcept
        {
            T const sin = std::sin(rad);
            T const cos = std::cos(rad);

            return {
                cos,  -sin, 0,    0,
                sin,  cos,  0,    0,
                0,    0,    1,    0,
                0,    0,    0,    1
            };
        }

        // Create a rotation matrix around the axis defined as an unit vector.
        static Matrix4 RotationAxis(Vector4<T> const& v, T rad) noexcept
        {
            T const sin = std::sin(rad);
            T const cos = std::cos(rad);
            T const ncos = T{1} - cos;

            return {
                cos + (v.x * v.x) * ncos,
                (v.x * v.y * ncos) - (v.z * sin),
                (v.x * v.z * ncos) + (v.y * sin),
                0,
                (v.y * v.x * ncos) + (v.z * sin),
                cos + ((v.y * v.y) * ncos),
                (v.y * v.z * ncos) - (v.x * sin),
                0,
                (v.z * v.x * ncos) - (v.y * sin),
                (v.z * v.y * ncos) + (v.x * sin),
                cos + ((v.z * v.z) * ncos),
                0,
                0,
                0,
                0,
                1
            };
        }

        // Create a scale matrix.
        static Matrix4 Scale(T x, T y, T z) noexcept
        {
            return {
                x, 0, 0, 0,
                0, y, 0, 0,
                0, 0, z, 0,
                0, 0, 0, 1
            };
        }

        // Create a translation matrix.
        static Matrix4 Translation(T x, T y, T z) noexcept
        {
            return {
                1, 0, 0, x,
                0, 1, 0, y,
                0, 0, 1, z,
                0, 0, 0, 1
            };
        }

        constexpr T& At(std::size_t row, std::size_t col) noexcept
        {
            assert(row < rows);
            assert(col < cols);

            return m[col][row];
        }

        constexpr T At(std::size_t row, std::size_t col) const noexcept
        {
            assert(row < rows);
            assert(col < cols);

            return m[col][row];
        }

        bool Invert() noexcept
        {
            return Matrix4::Invert(*this, *this);
        }

        void Transpose() noexcept
        {
            std::swap(At(1, 0), At(0, 1));
            std::swap(At(2, 0), At(0, 2));
            std::swap(At(3, 0), At(0, 3));
            std::swap(At(2, 1), At(1, 2));
            std::swap(At(3, 1), At(1, 3));
            std::swap(At(3, 2), At(2, 3));
        }

        T Determinant() const noexcept
        {
            T const t0 = (At(2, 2) * At(3, 3)) - (At(2, 3) * At(3, 2));
            T const t1 = (At(2, 3) * At(3, 1)) - (At(2, 1) * At(3, 3));
            T const t2 = (At(2, 1) * At(3, 2)) - (At(2, 2) * At(3, 1));

            T const det00 {
                (At(1, 1) * t0) +
                (At(1, 2) * t1) +
                (At(1, 3) * t2)
            };

            T const det10 {
                (At(0, 1) * t0) +
                (At(0, 2) * t1) +
                (At(0, 3) * t2)
            };

            T const det20 {
                (At(0, 1) * (At(1, 2) * At(3, 3) - At(1, 3) * At(3, 2))) +
                (At(0, 2) * (At(1, 3) * At(3, 1) - At(1, 1) * At(3, 3))) +
                (At(0, 3) * (At(1, 1) * At(3, 2) - At(1, 2) * At(3, 1)))
            };

            T const det30 {
                (At(0, 1) * (At(1, 2) * At(2, 3) - At(1, 3) * At(2, 2))) +
                (At(0, 2) * (At(1, 3) * At(2, 1) - At(1, 1) * At(2, 3))) +
                (At(0, 3) * (At(1, 1) * At(2, 2) - At(1, 2) * At(2, 1)))
            };

            return (At(0, 0) * det00) - (At(1, 0) * det10) + (At(2, 0) * det20) - (At(3, 0) * det30);
        }

        // Apply transformations (this * v).
        // Keep in mind that cumuleted transformations are applied "from right to left" order.
        Vector4<T> Transform(Vector4<T> const& v) const noexcept
        {
            return (*this) * v;
        }

        void AppendTransformations(Matrix4 const& t) noexcept
        {
            *this = t * (*this);
        }

        void PrependTransformations(Matrix4 const& t) noexcept
        {
            *this = (*this) * t;
        }

        void AppendTranslation(T x, T y, T z) noexcept
        {
            At(0, 3) += (At(0, 0) * x) + (At(0, 1) * y) + (At(0, 2) * z);
            At(1, 3) += (At(1, 0) * x) + (At(1, 1) * y) + (At(1, 2) * z);
            At(2, 3) += (At(2, 0) * x) + (At(2, 1) * y) + (At(2, 2) * z);
            At(3, 3) += (At(3, 0) * x) + (At(3, 1) * y) + (At(3, 2) * z);
        }

        void PrependTranslation(T x, T y, T z) noexcept
        {
            At(0, 0) += x * At(3, 0);
            At(0, 1) += x * At(3, 1);
            At(0, 2) += x * At(3, 2);
            At(0, 3) += x * At(3, 3);
            At(1, 0) += y * At(3, 0);
            At(1, 1) += y * At(3, 1);
            At(1, 2) += y * At(3, 2);
            At(1, 3) += y * At(3, 3);
            At(2, 0) += z * At(3, 0);
            At(2, 1) += z * At(3, 1);
            At(2, 2) += z * At(3, 2);
            At(2, 3) += z * At(3, 3);
        }

        void AppendScaling(T x, T y, T z) noexcept
        {
            At(0, 0) *= x;
            At(1, 0) *= x;
            At(2, 0) *= x;
            At(0, 1) *= y;
            At(1, 1) *= y;
            At(2, 1) *= y;
            At(0, 2) *= z;
            At(1, 2) *= z;
            At(2, 2) *= z;
        }

        void PrependScaling(T x, T y, T z) noexcept
        {
            At(0, 0) *= x;
            At(0, 1) *= x;
            At(0, 2) *= x;
            At(0, 3) *= x;
            At(1, 0) *= y;
            At(1, 1) *= y;
            At(1, 2) *= y;
            At(1, 3) *= y;
            At(2, 0) *= z;
            At(2, 1) *= z;
            At(2, 2) *= z;
            At(2, 3) *= z;
        }

        // Multiply.
        Matrix4 operator*(Matrix4 const& r) const noexcept
        {
            return {
                Mul(*this, r, 0, 0),
                Mul(*this, r, 0, 1),
                Mul(*this, r, 0, 2),
                Mul(*this, r, 0, 3),
                Mul(*this, r, 1, 0),
                Mul(*this, r, 1, 1),
                Mul(*this, r, 1, 2),
                Mul(*this, r, 1, 3),
                Mul(*this, r, 2, 0),
                Mul(*this, r, 2, 1),
                Mul(*this, r, 2, 2),
                Mul(*this, r, 2, 3),
                Mul(*this, r, 3, 0),
                Mul(*this, r, 3, 1),
                Mul(*this, r, 3, 2),
                Mul(*this, r, 3, 3),
            };
        }

        // Matrix4 * column-major vector (vector transformation).
        Vector4<T> operator*(Vector4<T> const& r) const noexcept
        {
            return {
                (At(0, 0) * r.x) + (At(0, 1) * r.y) + (At(0, 2) * r.z) + (At(0, 3) * r.w),
                (At(1, 0) * r.x) + (At(1, 1) * r.y) + (At(1, 2) * r.z) + (At(1, 3) * r.w),
                (At(2, 0) * r.x) + (At(2, 1) * r.y) + (At(2, 2) * r.z) + (At(2, 3) * r.w),
                (At(3, 0) * r.x) + (At(3, 1) * r.y) + (At(3, 2) * r.z) + (At(3, 3) * r.w)
            };
        }

        // Swap rows.
        void SwapRows(int r1, int r2) noexcept
        {
            std::swap(At(r1, 0), At(r2, 0));
            std::swap(At(r1, 1), At(r2, 1));
            std::swap(At(r1, 2), At(r2, 2));
            std::swap(At(r1, 3), At(r2, 3));
        }

        // Add a multiplied src row to a dest row.
        void AddRow(int src_row, int dst_row, T multiplier) noexcept
        {
            At(dst_row, 0) += (At(src_row, 0) * multiplier);
            At(dst_row, 1) += (At(src_row, 1) * multiplier);
            At(dst_row, 2) += (At(src_row, 2) * multiplier);
            At(dst_row, 3) += (At(src_row, 3) * multiplier);
        }

        // Multiply all entries of a row by the scalar.
        void MulRow(int row, T value) noexcept
        {
            At(row, 0) *= value;
            At(row, 1) *= value;
            At(row, 2) *= value;
            At(row, 3) *= value;
        }

    private:
        // Store inverse matrix 'm' to the output matrix 'o'
        static bool Invert(Matrix4 const& m, Matrix4& o) noexcept
        {
            // Precomputationed values.
            T const t00 = (m.At(2, 2) * m.At(3, 3)) - (m.At(2, 3) * m.At(3, 2));
            T const t01 = (m.At(2, 3) * m.At(3, 0)) - (m.At(2, 0) * m.At(3, 3));
            T const t02 = (m.At(2, 1) * m.At(3, 2)) - (m.At(2, 2) * m.At(3, 1));
            T const t03 = (m.At(2, 3) * m.At(3, 1)) - (m.At(2, 1) * m.At(3, 3));
            T const t04 = (m.At(2, 0) * m.At(3, 1)) - (m.At(2, 1) * m.At(3, 0));
            T const t05 = (m.At(2, 0) * m.At(3, 2)) - (m.At(2, 2) * m.At(3, 0));
            T const t06 = (m.At(2, 1) * m.At(3, 3)) - (m.At(2, 3) * m.At(3, 1));
            T const t07 = (m.At(2, 2) * m.At(3, 0)) - (m.At(2, 0) * m.At(3, 2));
            T const t08 = (m.At(1, 2) * m.At(3, 3)) - (m.At(1, 3) * m.At(3, 2));
            T const t09 = (m.At(1, 1) * m.At(3, 2)) - (m.At(1, 2) * m.At(3, 1));
            T const t10 = (m.At(1, 0) * m.At(3, 1)) - (m.At(1, 1) * m.At(3, 0));
            T const t11 = (m.At(1, 2) * m.At(2, 3)) - (m.At(1, 3) * m.At(2, 2));
            T const t12 = (m.At(1, 1) * m.At(2, 2)) - (m.At(1, 2) * m.At(2, 1));
            T const t13 = (m.At(1, 3) * m.At(2, 0)) - (m.At(1, 0) * m.At(2, 3));
            T const t14 = (m.At(1, 0) * m.At(2, 1)) - (m.At(1, 1) * m.At(2, 0));
            T const t15 = (m.At(1, 3) * m.At(3, 0)) - (m.At(1, 0) * m.At(3, 3));

            // Minors.
            T const det00 = (m.At(1, 1) * t00) + (m.At(1, 2) * t03) + (m.At(1, 3) * t02);
            T const det01 = (m.At(1, 0) * t00) + (m.At(1, 2) * t01) + (m.At(1, 3) * t05);
            T const det02 = (m.At(1, 0) * t06) + (m.At(1, 1) * t01) + (m.At(1, 3) * t04);
            T const det03 = (m.At(1, 0) * t02) + (m.At(1, 1) * t07) + (m.At(1, 2) * t04);
            T const det10 = (m.At(0, 1) * t00) + (m.At(0, 2) * t03) + (m.At(0, 3) * t02);
            T const det11 = (m.At(0, 0) * t00) + (m.At(0, 2) * t01) + (m.At(0, 3) * t05);
            T const det12 = (m.At(0, 0) * t06) + (m.At(0, 1) * t01) + (m.At(0, 3) * t04);
            T const det13 = (m.At(0, 0) * t02) + (m.At(0, 1) * t07) + (m.At(0, 2) * t04);
            T const det20 = (m.At(0, 1) * t08) + (m.At(0, 3) * t09) + (m.At(0, 2) * ((m.At(1, 3) * m.At(3, 1)) - (m.At(1, 1) * m.At(3, 3))));
            T const det21 = (m.At(0, 0) * t08) + (m.At(0, 2) * t15) + (m.At(0, 3) * ((m.At(1, 0) * m.At(3, 2)) - (m.At(1, 2) * m.At(3, 0))));
            T const det22 = (m.At(0, 1) * t15) + (m.At(0, 3) * t10) + (m.At(0, 0) * ((m.At(1, 1) * m.At(3, 3)) - (m.At(1, 3) * m.At(3, 1))));
            T const det23 = (m.At(0, 0) * t09) + (m.At(0, 2) * t10) + (m.At(0, 1) * ((m.At(1, 2) * m.At(3, 0)) - (m.At(1, 0) * m.At(3, 2))));
            T const det30 = (m.At(0, 1) * t11) + (m.At(0, 3) * t12) + (m.At(0, 2) * ((m.At(1, 3) * m.At(2, 1)) - (m.At(1, 1) * m.At(2, 3))));
            T const det31 = (m.At(0, 0) * t11) + (m.At(0, 2) * t13) + (m.At(0, 3) * ((m.At(1, 0) * m.At(2, 2)) - (m.At(1, 2) * m.At(2, 0))));
            T const det32 = (m.At(0, 1) * t13) + (m.At(0, 3) * t14) + (m.At(0, 0) * ((m.At(1, 1) * m.At(2, 3)) - (m.At(1, 3) * m.At(2, 1))));
            T const det33 = (m.At(0, 0) * t12) + (m.At(0, 2) * t14) + (m.At(0, 1) * ((m.At(1, 2) * m.At(2, 0)) - (m.At(1, 0) * m.At(2, 2))));

            // The matrix determinant.
            T const det = (m.At(0, 0) * det00) - (m.At(1, 0) * det10) + (m.At(2, 0) * det20) - (m.At(3, 0) * det30);

            // The matrix is singular (not invertible).
            if (det == 0)
            {
                return false;
            }

            // Set inverse entries.
            o.At(0, 0) = det00 / det;
            o.At(0, 1) = -det10 / det;
            o.At(0, 2) = det20 / det;
            o.At(0, 3) = -det30 / det;
            o.At(1, 0) = -det01 / det;
            o.At(1, 1) = det11 / det;
            o.At(1, 2) = -det21 / det;
            o.At(1, 3) = det31 / det;
            o.At(2, 0) = det02 / det;
            o.At(2, 1) = -det12 / det;
            o.At(2, 2) = det22 / det;
            o.At(2, 3) = -det32 / det;
            o.At(3, 0) = -det03 / det;
            o.At(3, 1) = det13 / det;
            o.At(3, 2) = -det23 / det;
            o.At(3, 3) = det33 / det;

            return true;
        }

        static constexpr T Mul(Matrix4 const& l, Matrix4 const& r, std::size_t row, std::size_t col) noexcept
        {
            return (
                (l.At(row, 0) * r.At(0, col)) +
                (l.At(row, 1) * r.At(1, col)) +
                (l.At(row, 2) * r.At(2, col)) +
                (l.At(row, 3) * r.At(3, col))
            );
        };

        static Matrix4 Mul(Matrix4 const& l, Matrix4 const& r) noexcept
        {
            return l * r;
        }

        // Row major entries.
        T m[cols][rows] {};
    };

} // Rc
