#include "gtest/gtest.h"
#include "base/color.h"

using namespace Rc;

TEST(Color_HSV, ConstructedFromRGB)
{
    Color a = Rgba(200, 100, 50, 255);
    auto b = static_cast<Hsva>(a);

    EXPECT_NEAR(b.h, 0.05f, 0.025f);
    EXPECT_NEAR(b.s, 0.75f, 0.025f);
    EXPECT_NEAR(b.v, 0.78f, 0.025f);
    EXPECT_NEAR(b.a, 1.0f, 0.001f);
}

TEST(Color_RGB, ConstructedFromHSV)
{
    Color a = Rgba(200, 100, 50, 255);
    Color b = Hsva(0.05f, 0.75f, 0.78f, 1.f);

    EXPECT_NEAR(a.r, b.r, 0.025f);
    EXPECT_NEAR(a.g, b.g, 0.025f);
    EXPECT_NEAR(a.b, b.b, 0.025f);
}