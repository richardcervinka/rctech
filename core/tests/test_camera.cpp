#include "gtest/gtest.h"
#include "core/camera.h"

using namespace Rc;
using namespace Rc::Gfx;

TEST(PerspectiveCamera, Projection_Perspective)
{
    PerspectiveCamera camera;
    camera.fov = Math::pi / 2.0;
    camera.z_near = 0.1;
    camera.z_far = 10.0;

    auto const m = camera.GetProjectionMatrix(800, 800);

    EXPECT_NEAR(m.At(0, 0), 1.0, 0.00001);
    EXPECT_NEAR(m.At(0, 1), 0.0, 0.00001);
    EXPECT_NEAR(m.At(0, 2), 0.0, 0.00001);
    EXPECT_NEAR(m.At(0, 3), 0.0, 0.00001);
    EXPECT_NEAR(m.At(1, 0), 0.0, 0.00001);
    EXPECT_NEAR(m.At(1, 1), 1.0, 0.00001);
    EXPECT_NEAR(m.At(1, 2), 0.0, 0.00001);
    EXPECT_NEAR(m.At(1, 3), 0.0, 0.00001);
    EXPECT_NEAR(m.At(2, 0), 0.0, 0.00001);
    EXPECT_NEAR(m.At(2, 1), 0.0, 0.00001);
    EXPECT_NEAR(m.At(2, 2), 0.010101, 0.00001);
    EXPECT_NEAR(m.At(2, 3), 0.101010, 0.00001);
    EXPECT_NEAR(m.At(3, 0), 0.0, 0.00001);
    EXPECT_NEAR(m.At(3, 1), 0.0, 0.00001);
    EXPECT_NEAR(m.At(3, 2), -1.0, 0.00001);
    EXPECT_NEAR(m.At(3, 3), 0.0, 0.00001);
}