#include "gtest/gtest.h"
#include "render/projection.h"
#include "base/math.h"

using namespace Rc;

TEST(Projection_Perspective, Create)
{
    auto const m = Render::CreatePerspectiveProjectionMatrix(800, 800, Math::pi / 2.0, 0.1, 10.0);

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