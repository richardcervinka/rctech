#include "gtest/gtest.h"
#include "base/math.h"

using namespace Rc;

TEST(QuaternionTest, Multiplication)
{
    Quaternion const q1 {1, 0, 0, 0};

    ASSERT_FLOAT_EQ(q1.Length(), 1);
    EXPECT_FLOAT_EQ((q1 * q1).Length(), 1);
}

TEST(QuaternionTest, Normalize)
{
    auto const q = Quaternion::Normalized({1, 2, 3, 4});

    EXPECT_FLOAT_EQ(q.Length(), 1.0f);
}

TEST(QuaternionTest, Conjugation)
{
    auto const q = Quaternion{1, 2, 3, 4};
    auto const a = q * Quaternion::Conjugated(q);
    auto const b = Quaternion::Conjugated(q) * q;

    EXPECT_FLOAT_EQ(a.w, b.w);
    EXPECT_FLOAT_EQ(a.x, b.x);
    EXPECT_FLOAT_EQ(a.y, b.y);
    EXPECT_FLOAT_EQ(a.z, b.z);
}

TEST(QuaternionTest, Inverse)
{
    auto const q = Quaternion::Normalized({1, 2, 3, 4});
    auto const i = Quaternion::Inversed(q);
    auto const c = Quaternion::Conjugated(q);

    EXPECT_FLOAT_EQ(i.w, c.w);
    EXPECT_FLOAT_EQ(i.x, c.x);
    EXPECT_FLOAT_EQ(i.y, c.y);
    EXPECT_FLOAT_EQ(i.z, c.z);
}

TEST(QuaternionTest, Dot)
{
    auto const q = Quaternion{1, 2, 3, 4};
    auto const qq = Quaternion::Dot(q, q);
    auto const sq = q.LengthSquare();

    EXPECT_FLOAT_EQ(qq, sq);
}

TEST(QuaternionTest, RotationX)
{
    auto const q = Quaternion::RotationX(Math::pi / 2.f);
    auto const m = q.ToMatrix<double>();
    auto const p = m.Transform(Vector4<double>{3, 2, 0, 1});

    EXPECT_NEAR(p.x, 3, 1e-6);
    EXPECT_NEAR(p.y, 0, 1e-6);
    EXPECT_NEAR(p.z, 2, 1e-6);
    EXPECT_NEAR(p.w, 1, 1e-6);
}

TEST(QuaternionTest, RotationY)
{
    auto const q = Quaternion::RotationY(Math::pi / 2.f);
    auto const m = q.ToMatrix<double>();
    auto const p = m.Transform(Vector4<double>{2, 3, 4, 1});

    EXPECT_NEAR(p.x, 4, 1e-6);
    EXPECT_NEAR(p.y, 3, 1e-6);
    EXPECT_NEAR(p.z, -2, 1e-6);
    EXPECT_NEAR(p.w, 1, 1e-6);
}

TEST(QuaternionTest, RotationZ)
{
    auto const q = Quaternion::RotationZ(Math::pi / 2.f);
    auto const m = q.ToMatrix<double>();
    auto const p = m.Transform(Vector4<double>{2, 3, 4, 1});

    EXPECT_NEAR(p.x, -3, 1e-6);
    EXPECT_NEAR(p.y, 2, 1e-6);
    EXPECT_NEAR(p.z, 4, 1e-6);
    EXPECT_NEAR(p.w, 1, 1e-6);
}

TEST(QuaternionTest, Slerp1)
{
    auto const a = Quaternion{1, 0, 0, 0};
    auto const b = Quaternion{-1, 0, 0, 0};
    auto const s = Quaternion::Slerp(a, b, 0.5);
    auto const m = s.ToMatrix<float>();
    auto const u = Vector4<float>{1, 0, 0, 1};
    auto const v = m.Transform(u);

    EXPECT_FLOAT_EQ(u.w, v.w);
    EXPECT_FLOAT_EQ(u.x, v.x);
    EXPECT_FLOAT_EQ(u.y, v.y);
    EXPECT_FLOAT_EQ(u.z, v.z);
}

TEST(QuaternionTest, Slerp2)
{
    auto const a = Quaternion{0.707f, 0.707f, 0, 0};
    auto const b = Quaternion{0.707f, 0, 0.707f, 0};
    auto const c = Quaternion{-0.707f, 0, -0.707f, 0};
    auto const sab = Quaternion::Slerp(a, b, 0.5);
    auto const sac = Quaternion::Slerp(a, c, 0.5);

    auto const mab = sab.ToMatrix<float>();
    auto const mac = sac.ToMatrix<float>();
    auto const v = Vector4<float>{1, 0, 0, 1};
    auto const vab = mab.Transform(v);
    auto const vac = mac.Transform(v);

    EXPECT_FLOAT_EQ(vab.w, vac.w);
    EXPECT_FLOAT_EQ(vab.x, vac.x);
    EXPECT_FLOAT_EQ(vab.y, vac.y);
    EXPECT_FLOAT_EQ(vab.z, vac.z);
}