#include "gtest/gtest.h"
#include "core/transformations.h"

using namespace Rc;
using namespace Rc::Gfx;

TEST(Transformations_Translation, TranslatePoint)
{
    Transformations t;
    t.x = 5;
    t.y = 6;
    t.z = 7;

    const auto p = Vector4<double>{1, 2, 3, 1};
    const auto m = t.GetTransformations();
    const auto q = m.Transform(p);

    EXPECT_NEAR(q.x, p.x + t.x, 1e-5);
    EXPECT_NEAR(q.y, p.y + t.y, 1e-5);
    EXPECT_NEAR(q.z, p.z + t.z, 1e-5);
    EXPECT_NEAR(q.w, 1, 1e-5);
}

TEST(Transformations_Scaling, ScalePoint)
{
    Transformations t;
    t.scale = 3;

    const auto p = Vector4<double>{1, 2, 3, 1};
    const auto m = t.GetTransformations();
    const auto q = m.Transform(p);

    EXPECT_NEAR(q.x, p.x * t.scale, 1e-5);
    EXPECT_NEAR(q.y, p.y * t.scale, 1e-5);
    EXPECT_NEAR(q.z, p.z * t.scale, 1e-5);
    EXPECT_NEAR(q.w, 1, 1e-5);
}

// TEST(Transformations_ScalingAndTranslation, ScaleAndTranslatePoint)
// {
//     Transformations t;
//     t.x = 5;
//     t.y = 6;
//     t.z = 7;
//     t.scale = 3;

//     const auto p = Vector4<double>{1, 2, 3, 1};
//     const auto m = t.GetTransformations();
//     const auto q = m.Transform(p);

//     EXPECT_NEAR(q.x, (p.x * t.scale) + t.x, 1e-5);
//     EXPECT_NEAR(q.y, (p.y * t.scale) + t.y, 1e-5);
//     EXPECT_NEAR(q.z, (p.z * t.scale) + t.z, 1e-5);
//     EXPECT_NEAR(q.w, 1, 1e-5);
// }

TEST(Transformations_Rotation, Yaw_RotateBy720Degrees)
{
    Transformations t;
    t.yaw = Math::pi * 4.0;

    const auto p = Vector4<double>{1, 2, 3, 1};
    const auto m = t.GetTransformations();
    const auto q = m.Transform(p);

    EXPECT_NEAR(q.x, p.x, 1e-5);
    EXPECT_NEAR(q.y, p.y, 1e-5);
    EXPECT_NEAR(q.z, p.z, 1e-5);
    EXPECT_NEAR(q.w, 1, 1e-5);
}

TEST(Transformations_Rotation, Yaw_RotateBy90Degrees)
{
    Transformations t;
    t.yaw = Math::pi / 2.0;

    const auto p = Vector4<double>{1, 2, 3, 1};
    const auto m = t.GetTransformations();
    const auto q = m.Transform(p);

    EXPECT_NEAR(q.x, p.z, 1e-5);
    EXPECT_NEAR(q.y, p.y, 1e-5);
    EXPECT_NEAR(q.z, -p.x, 1e-5);
    EXPECT_NEAR(q.w, 1, 1e-5);
}

TEST(Transformations_Rotation, Pitch_RotateBy90Degrees)
{
    Transformations t;
    t.pitch = Math::pi / 2.0;

    const auto p = Vector4<double>{1, 2, 3, 1};
    const auto m = t.GetTransformations();
    const auto q = m.Transform(p);

    EXPECT_NEAR(q.x, p.x, 1e-5);
    EXPECT_NEAR(q.y, -p.z, 1e-5);
    EXPECT_NEAR(q.z, p.y, 1e-5);
    EXPECT_NEAR(q.w, 1, 1e-5);
}

TEST(Transformations_Rotation, Roll_RotateBy90Degrees)
{
    Transformations t;
    t.roll = Math::pi / 2.0;

    const auto p = Vector4<double>{1, 2, 3, 1};
    const auto m = t.GetTransformations();
    const auto q = m.Transform(p);

    EXPECT_NEAR(q.x, -p.y, 1e-5);
    EXPECT_NEAR(q.y, p.x, 1e-5);
    EXPECT_NEAR(q.z, p.z, 1e-5);
    EXPECT_NEAR(q.w, 1, 1e-5);
}

TEST(Transformations_Rotation, YawAndPitch_RotateBy90Degrees)
{
    Transformations t;
    t.yaw = Math::pi / 2.0;
    t.pitch = Math::pi / 2.0;

    const auto p = Vector4<double>{1, 2, 3, 1};
    const auto m = t.GetTransformations();
    const auto q = m.Transform(p);

    EXPECT_DOUBLE_EQ(q.x, p.y);
    EXPECT_DOUBLE_EQ(q.y, -p.z);
    EXPECT_DOUBLE_EQ(q.z, -p.x);
    EXPECT_DOUBLE_EQ(q.w, 1);
}

TEST(Transformations_Rotation, PitchAndRoll_RotateBy90Degrees)
{
    Transformations t;
    t.pitch = Math::pi / 2.0;
    t.roll = Math::pi / 2.0;

    const auto p = Vector4<double>{1, 2, 3, 1};
    const auto m = t.GetTransformations();
    const auto q = m.Transform(p);

    EXPECT_DOUBLE_EQ(q.x, -p.y);
    EXPECT_DOUBLE_EQ(q.y, -p.z);
    EXPECT_DOUBLE_EQ(q.z, p.x);
    EXPECT_DOUBLE_EQ(q.w, 1);
}

TEST(Transformations_RotationBy180DegreesAndTranslation, PointAtNegativeXZ)
{
    Transformations t;
    t.yaw = Math::pi;
    t.x = 1;
    t.y = 2;
    t.z = 3;

    const auto p = Vector4<double>{0, 0, 0, 1};
    const auto m = t.GetTransformations();
    const auto q = m.Transform(p);

    EXPECT_NEAR(q.x, -t.x, 1e-5);
    EXPECT_NEAR(q.y, t.y, 1e-5);
    EXPECT_NEAR(q.z, -t.z, 1e-5);
    EXPECT_NEAR(q.w, 1, 1e-5);
}