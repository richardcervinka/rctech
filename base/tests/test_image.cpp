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

    ASSERT_EQ(mip.Width(), 1);
    ASSERT_EQ(mip.Height(), 1);
    ASSERT_EQ(data.size(), 1);

    EXPECT_NE((data[0, 0]).r, 127);
    EXPECT_NE((data[0, 0]).g, 127);
    EXPECT_NE((data[0, 0]).b, 127);
    EXPECT_EQ((data[0, 0]).a, 127);
}

TEST(ImageTest, CreateMip_SourceHeightEquals1)
{
    Image const image(4, 1, {
        11, 12, 13, 0,
        51, 52, 53, 0,
        101, 102, 103, 0,
        151, 152, 153, 0
    });

    auto const mip = image.GenerateMip();
    auto const data = mip.Data();

    ASSERT_EQ(mip.Width(), 2);
    ASSERT_EQ(mip.Height(), 1);
    ASSERT_EQ(data.size(), 2);

    EXPECT_EQ((data[0, 0]).r, 36);
    EXPECT_EQ((data[0, 0]).g, 37);
    EXPECT_EQ((data[0, 0]).b, 38);
    EXPECT_EQ((data[0, 0]).a, 0);

    EXPECT_EQ((data[0, 1]).r, 129);
    EXPECT_EQ((data[0, 1]).g, 130);
    EXPECT_EQ((data[0, 1]).b, 131);
    EXPECT_EQ((data[0, 1]).a, 0);
}

TEST(ImageTest, CreateMip_SourceWidthEquals1)
{
    Image const image(1, 4, {
        11, 12, 13, 0,
        51, 52, 53, 0,
        101, 102, 103, 0,
        151, 152, 153, 0
    });

    auto const mip = image.GenerateMip();
    auto const data = mip.Data();

    ASSERT_EQ(mip.Width(), 1);
    ASSERT_EQ(mip.Height(), 2);
    ASSERT_EQ(data.size(), 2);

    EXPECT_EQ((data[0, 0]).r, 36);
    EXPECT_EQ((data[0, 0]).g, 37);
    EXPECT_EQ((data[0, 0]).b, 38);
    EXPECT_EQ((data[0, 0]).a, 0);

    EXPECT_EQ((data[1, 0]).r, 129);
    EXPECT_EQ((data[1, 0]).g, 130);
    EXPECT_EQ((data[1, 0]).b, 131);
    EXPECT_EQ((data[1, 0]).a, 0);
}