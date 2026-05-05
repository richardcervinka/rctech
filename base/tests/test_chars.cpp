#include "gtest/gtest.h"
#include "base/chars.h"

using namespace Rc;

TEST(CharsToNumber, ValidInput)
{
    EXPECT_TRUE(Chars::To<int>("0").has_value());
    EXPECT_TRUE(Chars::To<int>("10").value());
    EXPECT_TRUE(Chars::To<float>("3.14").value());
    EXPECT_TRUE(Chars::To<int>("0002").value());
}

TEST(CharsToNumber, InvalidInput)
{
    EXPECT_FALSE(Chars::To<uint8_t>("256").has_value());
    EXPECT_FALSE(Chars::To<uint8_t>("-10").has_value());
    EXPECT_FALSE(Chars::To<int>("+10").has_value());
    EXPECT_FALSE(Chars::To<int>("ab").has_value());
    EXPECT_FALSE(Chars::To<int>("1b").has_value());
    EXPECT_FALSE(Chars::To<int>("-0").value());
}
