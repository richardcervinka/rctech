#include "gtest/gtest.h"
#include "base/image.h"

using namespace Rc;

TEST(ImageTest, CreateMip)
{
    Image const image(2, 2, {
        111, 112, 113, 0,
        121, 122, 123, 0,
        131, 132, 133, 0,
        141, 142, 143, 0
    });

    auto const mip = image.GenerateMip();
    auto const data = mip.Data();
    auto const value = data[0, 0];

    ASSERT_EQ(mip.Width(), 1);
    ASSERT_EQ(mip.Height(), 1);
    ASSERT_EQ(data.size(), 1);

    EXPECT_EQ(value.r, 126);
    EXPECT_EQ(value.g, 127);
    EXPECT_EQ(value.b, 128);
    EXPECT_EQ(value.a, 0);
}

TEST(ImageTest, CreateMip_AlphaIsLinear)
{
    Image const image(2, 2, {
        0, 255, 0, 255,
        0, 255, 0, 255,
        255, 0, 255, 0,
        255, 0, 255, 0
    });

    auto const mip = image.GenerateMip();
    auto const data = mip.Data();
    auto const value = data[0, 0];

    ASSERT_EQ(mip.Width(), 1);
    ASSERT_EQ(mip.Height(), 1);
    ASSERT_EQ(data.size(), 1);

    EXPECT_NE(value.r, 127);
    EXPECT_NE(value.g, 127);
    EXPECT_NE(value.b, 127);
    EXPECT_EQ(value.a, 127);
}