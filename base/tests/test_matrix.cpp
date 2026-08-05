#include "gtest/gtest.h"
#include "base/math.h"

using namespace Rc;
using namespace Rc::Math;

TEST(MatrixTest, Construct)
{}

TEST(MatrixTest, To)
{
    auto const m = Matrix4<double>::Identity().To<float>();

    EXPECT_EQ(m.At(0, 0), 1);
    EXPECT_EQ(m.At(0, 1), 0);
    EXPECT_EQ(m.At(0, 2), 0);
    EXPECT_EQ(m.At(0, 3), 0);
    EXPECT_EQ(m.At(1, 0), 0);
    EXPECT_EQ(m.At(1, 1), 1);
    EXPECT_EQ(m.At(1, 2), 0);
    EXPECT_EQ(m.At(1, 3), 0);
    EXPECT_EQ(m.At(2, 0), 0);
    EXPECT_EQ(m.At(2, 1), 0);
    EXPECT_EQ(m.At(2, 2), 1);
    EXPECT_EQ(m.At(2, 3), 0);
    EXPECT_EQ(m.At(3, 0), 0);
    EXPECT_EQ(m.At(3, 1), 0);
    EXPECT_EQ(m.At(3, 2), 0);
    EXPECT_EQ(m.At(3, 3), 1);
}

TEST(MatrixTest, At)
{
    Matrix4<double> const m {
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 1,
        0, 0, 0, 0
    };

    EXPECT_EQ(m.At(2, 3), 1);
    EXPECT_EQ(m.At(3, 2), 0);
}

TEST(MatrixTest, Identity)
{
    auto const m = Matrix4<double>::Identity();
    
    EXPECT_EQ(m.At(0, 0), 1);
    EXPECT_EQ(m.At(0, 1), 0);
    EXPECT_EQ(m.At(0, 2), 0);
    EXPECT_EQ(m.At(0, 3), 0);
    EXPECT_EQ(m.At(1, 0), 0);
    EXPECT_EQ(m.At(1, 1), 1);
    EXPECT_EQ(m.At(1, 2), 0);
    EXPECT_EQ(m.At(1, 3), 0);
    EXPECT_EQ(m.At(2, 0), 0);
    EXPECT_EQ(m.At(2, 1), 0);
    EXPECT_EQ(m.At(2, 2), 1);
    EXPECT_EQ(m.At(2, 3), 0);
    EXPECT_EQ(m.At(3, 0), 0);
    EXPECT_EQ(m.At(3, 1), 0);
    EXPECT_EQ(m.At(3, 2), 0);
    EXPECT_EQ(m.At(3, 3), 1);
}

TEST(MatrixTest, Transpose)
{
    Matrix4<float> m {
        1,  2,  3,  4,
        5,  6,  7,  8,
        9,  10, 11, 12,
        13, 14, 15, 16
    };

    m.Transpose();

    EXPECT_FLOAT_EQ(m.At(0, 0), 1);
    EXPECT_FLOAT_EQ(m.At(0, 1), 5);
    EXPECT_FLOAT_EQ(m.At(0, 2), 9);
    EXPECT_FLOAT_EQ(m.At(0, 3), 13);
    EXPECT_FLOAT_EQ(m.At(1, 0), 2);
    EXPECT_FLOAT_EQ(m.At(1, 1), 6);
    EXPECT_FLOAT_EQ(m.At(1, 2), 10);
    EXPECT_FLOAT_EQ(m.At(1, 3), 14);
    EXPECT_FLOAT_EQ(m.At(2, 0), 3);
    EXPECT_FLOAT_EQ(m.At(2, 1), 7);
    EXPECT_FLOAT_EQ(m.At(2, 2), 11);
    EXPECT_FLOAT_EQ(m.At(2, 3), 15);
    EXPECT_FLOAT_EQ(m.At(3, 0), 4);
    EXPECT_FLOAT_EQ(m.At(3, 1), 8);
    EXPECT_FLOAT_EQ(m.At(3, 2), 12);
    EXPECT_FLOAT_EQ(m.At(3, 3), 16);
}

TEST(MatrixTest, ConstructTranspose)
{
    auto const m = Matrix4<float>::Transpose({
        1,  2,  3,  4,
        5,  6,  7,  8,
        9,  10, 11, 12,
        13, 14, 15, 16
    });

    EXPECT_FLOAT_EQ(m.At(0, 0), 1);
    EXPECT_FLOAT_EQ(m.At(0, 1), 5);
    EXPECT_FLOAT_EQ(m.At(0, 2), 9);
    EXPECT_FLOAT_EQ(m.At(0, 3), 13);
    EXPECT_FLOAT_EQ(m.At(1, 0), 2);
    EXPECT_FLOAT_EQ(m.At(1, 1), 6);
    EXPECT_FLOAT_EQ(m.At(1, 2), 10);
    EXPECT_FLOAT_EQ(m.At(1, 3), 14);
    EXPECT_FLOAT_EQ(m.At(2, 0), 3);
    EXPECT_FLOAT_EQ(m.At(2, 1), 7);
    EXPECT_FLOAT_EQ(m.At(2, 2), 11);
    EXPECT_FLOAT_EQ(m.At(2, 3), 15);
    EXPECT_FLOAT_EQ(m.At(3, 0), 4);
    EXPECT_FLOAT_EQ(m.At(3, 1), 8);
    EXPECT_FLOAT_EQ(m.At(3, 2), 12);
    EXPECT_FLOAT_EQ(m.At(3, 3), 16);
}

TEST(MatrixTest, Invert)
{
    Matrix4<float> m {
        1,  0,  2, -1,
        3,  0,  0,  5,
        2,  1,  4, -3,
        1,  0,  5,  0
    };

    m.Invert();

    EXPECT_FLOAT_EQ(m.At(0, 0), 5.f / 6.f);
    EXPECT_FLOAT_EQ(m.At(0, 1), 1.f / 6.f);
    EXPECT_FLOAT_EQ(m.At(0, 2), 0);
    EXPECT_FLOAT_EQ(m.At(0, 3), -1.f / 3.f);
    EXPECT_FLOAT_EQ(m.At(1, 0), -5.f / 2.f);
    EXPECT_FLOAT_EQ(m.At(1, 1), 1.f / 10.f);
    EXPECT_FLOAT_EQ(m.At(1, 2), 1);
    EXPECT_FLOAT_EQ(m.At(1, 3), 1.f / 5.f);
    EXPECT_FLOAT_EQ(m.At(2, 0), -1.f / 6.f);
    EXPECT_FLOAT_EQ(m.At(2, 1), -1.f / 30.f);
    EXPECT_FLOAT_EQ(m.At(2, 2), 0);
    EXPECT_FLOAT_EQ(m.At(2, 3), 4.f / 15.f);
    EXPECT_FLOAT_EQ(m.At(3, 0), -1.f / 2.f);
    EXPECT_FLOAT_EQ(m.At(3, 1), 1.f / 10.f);
    EXPECT_FLOAT_EQ(m.At(3, 2), 0);
    EXPECT_FLOAT_EQ(m.At(3, 3), 1.f / 5.f);
}

TEST(MatrixTest, SwapRows)
{
    Matrix4<float> m {
        11, 12, 13, 14,
        21, 22, 23, 24,
        31, 32, 33, 34,
        41, 42, 43, 44
    };

    m.SwapRows(1, 3);

    EXPECT_EQ(m.At(1, 0), 41);
    EXPECT_EQ(m.At(1, 1), 42);
    EXPECT_EQ(m.At(1, 2), 43);
    EXPECT_EQ(m.At(1, 3), 44);
    EXPECT_EQ(m.At(3, 0), 21);
    EXPECT_EQ(m.At(3, 1), 22);
    EXPECT_EQ(m.At(3, 2), 23);
    EXPECT_EQ(m.At(3, 3), 24);
}

TEST(MatrixTest, Transform)
{
    auto m = Matrix4<double>::Identity();
    m.AppendTranslation(2, 3, 1);
    m.PrependScaling(2, 2, 1);
    
    auto v = m.Transform({1, 2, 0, 1});

    EXPECT_DOUBLE_EQ(v.x, 4);
    EXPECT_DOUBLE_EQ(v.y, 7);
    EXPECT_DOUBLE_EQ(v.z, 1);
}

TEST(MatrixTest, Translation)
{
    auto const m = Matrix4<double>::Translation(10, -20, 30);
    auto const v = m.Transform({1, 2, 3, 1});

    EXPECT_DOUBLE_EQ(v.x, 11);
    EXPECT_DOUBLE_EQ(v.y, -18);
    EXPECT_DOUBLE_EQ(v.z, 33);
    EXPECT_DOUBLE_EQ(v.w, 1);
}

TEST(MatrixTest, AppendTranslation)
{
    auto m = Matrix4<double>::RotationZ(Math::pi / 2.f);
    m.AppendTranslation(4, 5, 6);
    auto const v = m.Transform({1, 2, 3, 1});

    EXPECT_NEAR(v.x, -7, 1e-6);
    EXPECT_NEAR(v.y, 5, 1e-6);
    EXPECT_NEAR(v.z, 9, 1e-6);
    EXPECT_NEAR(v.w, 1, 1e-6);
}

TEST(MatrixTest, PrependTranslation)
{
    auto m = Matrix4<double>::RotationZ(Math::pi / 2.f);

    // Prepend translation is equivalent to
    // m.At(0, 3) = 4;
    // m.At(1, 3) = 5;
    // m.At(2, 3) = 6;
    m.PrependTranslation(4, 5, 6);

    auto const v = m.Transform({1, 2, 3, 1});

    EXPECT_NEAR(v.x, 2, 1e-6);
    EXPECT_NEAR(v.y, 6, 1e-6);
    EXPECT_NEAR(v.z, 9, 1e-6);
    EXPECT_NEAR(v.w, 1, 1e-6);
}

TEST(MatrixTest, Scale)
{
    auto const m = Matrix4<double>::Scale(2, 3, 4);
    auto const v = m.Transform({-10, 100, 1000, 1});

    EXPECT_DOUBLE_EQ(v.x, -20);
    EXPECT_DOUBLE_EQ(v.y, 300);
    EXPECT_DOUBLE_EQ(v.z, 4000);
    EXPECT_DOUBLE_EQ(v.w, 1);
}

TEST(MatrixTest, RotationX)
{
    auto const m = Matrix4<double>::RotationX(Math::pi / 2.0f);
    auto const v = m.Transform({3, 2, 1, 1});

    EXPECT_NEAR(v.x, 3, 1e-6);
    EXPECT_NEAR(v.y, -1, 1e-6);
    EXPECT_NEAR(v.z, 2, 1e-6);
    EXPECT_NEAR(v.w, 1, 1e-6);
}

TEST(MatrixTest, RotationY)
{
    auto const m = Matrix4<double>::RotationY(Math::pi / 2.0f);
    auto const v = m.Transform({1, 3, 2, 1});

    EXPECT_NEAR(v.x, 2, 1e-6);
    EXPECT_NEAR(v.y, 3, 1e-6);
    EXPECT_NEAR(v.z, -1, 1e-6);
    EXPECT_NEAR(v.w, 1, 1e-6);
}

TEST(MatrixTest, RotationZ)
{
    auto const m = Matrix4<double>::RotationZ(Math::pi / 2.0f);
    auto const v = m.Transform({1, 2, 3, 1});

    EXPECT_NEAR(v.x, -2, 1e-6);
    EXPECT_NEAR(v.y, 1, 1e-6);
    EXPECT_NEAR(v.z, 3, 1e-6);
    EXPECT_NEAR(v.w, 1, 1e-6);
}