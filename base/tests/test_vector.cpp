#include "gtest/gtest.h"
#include "base/math.h"

using namespace Rc;

TEST(Vector4_Addition, ComponentWise)
{
    auto v = Vector4{1, 2, 3, 4} + Vector4{4, 3, 2, 1};

    EXPECT_FLOAT_EQ(v.x, 5.0f);
    EXPECT_FLOAT_EQ(v.y, 5.0f);
    EXPECT_FLOAT_EQ(v.z, 5.0f);
    EXPECT_FLOAT_EQ(v.w, 5.0f);
}

TEST(Vector4_Subtraction, ComponentWise)
{
    auto v = Vector4{4, 3, 2, 1} - Vector4{1, 2, 3, 4};

    EXPECT_FLOAT_EQ(v.x, 3.0f);
    EXPECT_FLOAT_EQ(v.y, 1.0f);
    EXPECT_FLOAT_EQ(v.z, -1.0f);
    EXPECT_FLOAT_EQ(v.w, -3.0f);
}

TEST(Vector4_Multiplication, ComponentsMultipliedByScalar)
{
    auto v = Vector4{1, 2, 3, 4} * 2.0f;

    EXPECT_FLOAT_EQ(v.x, 2.0f);
    EXPECT_FLOAT_EQ(v.y, 4.0f);
    EXPECT_FLOAT_EQ(v.z, 6.0f);
    EXPECT_FLOAT_EQ(v.w, 8.0f);
}

TEST(Vector4_Cross, ComponentWise)
{
    auto v = Vector4<float>::Cross({2, 5, 6, 1}, {9, 7, 3, 1});

    EXPECT_FLOAT_EQ(v.x, -27.0f);
    EXPECT_FLOAT_EQ(v.y, 48.0f);
    EXPECT_FLOAT_EQ(v.z, -31.0f);
    EXPECT_FLOAT_EQ(v.w, 0.0f);
}

TEST(Vector4_Normalize, LengthEqualsOne)
{
    Vector4 v {1, 2, 3, 0};
    v.Normalize();

    EXPECT_FLOAT_EQ(v.Length(), 1.0f);
    EXPECT_FLOAT_EQ(v.w, 0.0f);
}

TEST(Vector4_Normal, ProducesUnitNormalWithWZero) {
    auto v = Vector4<float>::Normal({2, 5, 6, 1}, {9, 7, 3, 1});

    EXPECT_NEAR(v.x, -0.4272f, 0.0001f);
    EXPECT_NEAR(v.y, 0.7595f, 0.0001f);
    EXPECT_NEAR(v.z, -0.4905f, 0.0001f);
    EXPECT_FLOAT_EQ(v.w, 0.0f);
}
