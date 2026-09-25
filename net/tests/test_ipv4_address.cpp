#include <gtest/gtest.h>
#include <gtest/gtest_prod.h>
#include "net/ipv4_address.h"

TEST(IPv4Address, ConstructFromOctets)
{
    EXPECT_EQ(Rc::IPv4::Address(1, 2, 3, 4).Value(), 0x01020304U);
}

TEST(IPv4Address, ConstructFromUint32)
{
    EXPECT_EQ(Rc::IPv4::Address(0x01020304U).Value(), 0x01020304U);
}

TEST(IPv4Address, ConstructFromString)
{
    EXPECT_EQ(Rc::IPv4::Address("1.2.3.4").Value(), 0x01020304U);
}

TEST(IPv4Address, ConstructFromInvalidStringThrowsException)
{
    EXPECT_ANY_THROW(Rc::IPv4::Address ip("01.2.3.4"));
    EXPECT_ANY_THROW(Rc::IPv4::Address ip("001.2.3.4"));
    EXPECT_ANY_THROW(Rc::IPv4::Address ip("1.02.3.4"));
    EXPECT_ANY_THROW(Rc::IPv4::Address ip("1.002.3.4"));
    EXPECT_ANY_THROW(Rc::IPv4::Address ip("1.2.03.4"));
    EXPECT_ANY_THROW(Rc::IPv4::Address ip("1.2.003.4"));
    EXPECT_ANY_THROW(Rc::IPv4::Address ip("1.2.3.04"));
    EXPECT_ANY_THROW(Rc::IPv4::Address ip("1.2.3.004"));

    EXPECT_ANY_THROW(Rc::IPv4::Address(""));
    EXPECT_ANY_THROW(Rc::IPv4::Address("1"));
    EXPECT_ANY_THROW(Rc::IPv4::Address("1.2.3"));
    EXPECT_ANY_THROW(Rc::IPv4::Address("1.2.3."));
    EXPECT_ANY_THROW(Rc::IPv4::Address("-1.2.3.4"));
    EXPECT_ANY_THROW(Rc::IPv4::Address("1.2.3.4.5"));
    EXPECT_ANY_THROW(Rc::IPv4::Address("256.2.3.4"));
    EXPECT_ANY_THROW(Rc::IPv4::Address("1.256.3.4"));
    EXPECT_ANY_THROW(Rc::IPv4::Address("1.2.256.4"));
    EXPECT_ANY_THROW(Rc::IPv4::Address("1.2.3.256"));
    EXPECT_ANY_THROW(Rc::IPv4::Address("..."));
    EXPECT_ANY_THROW(Rc::IPv4::Address(".2.4.4"));
}

TEST(IPv4Address, GetOctets)
{
    Rc::IPv4::Address ip(1, 2, 3, 4);
    EXPECT_EQ(ip.Bytes().at(0), 1);
    EXPECT_EQ(ip.Bytes().at(1), 2);
    EXPECT_EQ(ip.Bytes().at(2), 3);
    EXPECT_EQ(ip.Bytes().at(3), 4);
    EXPECT_EQ(ip.Value(), 0x01020304U);
}

TEST(IPv4Address, ToString)
{
    EXPECT_EQ(Rc::IPv4::Address(1, 2, 3, 4).Str(), "1.2.3.4");
    EXPECT_EQ(Rc::IPv4::Address().Str(), "0.0.0.0");
}

TEST(IPv4Address, Comparsion)
{
  EXPECT_TRUE(Rc::IPv4::Address(1, 2, 3, 4) == Rc::IPv4::Address(1, 2, 3, 4));
  EXPECT_TRUE(Rc::IPv4::Address(1, 2, 3, 4) != Rc::IPv4::Address(1, 2, 3, 5));
}