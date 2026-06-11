#include "gtest/gtest.h"
#include "base/str.h"

using namespace Rc;

TEST(Str_To, ValidInput)
{
    EXPECT_TRUE(Str::To<int>("0").has_value());
    EXPECT_TRUE(Str::To<int>("10").value());
    EXPECT_TRUE(Str::To<float>("3.14").value());
    EXPECT_TRUE(Str::To<int>("0002").value());
}

TEST(Str_To, InvalidInput)
{
    // EXPECT_FALSE(Str::To<uint8_t>("256").has_value());
    // EXPECT_FALSE(Str::To<uint8_t>("-10").has_value());
    EXPECT_FALSE(Str::To<int>("+10").has_value());
    EXPECT_FALSE(Str::To<int>("ab").has_value());
    EXPECT_FALSE(Str::To<int>("1b").has_value());
    EXPECT_FALSE(Str::To<int>("-0").value());
}
