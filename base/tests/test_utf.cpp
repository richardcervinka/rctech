#include "gtest/gtest.h"
#include "base/utf.h"

using namespace Rc;

TEST(Utf8_Iterator, SingleByteCharacters)
{
    std::string s = "ABC";
    Utf8::Iterator it {s};
    EXPECT_EQ(*it, U'A');
    ++it;
    EXPECT_EQ(*it, U'B');
    ++it;
    EXPECT_EQ(*it, U'C');
    ++it;
    EXPECT_TRUE(it == Utf8::Sentinel());
}

TEST(Utf8_Iterator, TwoByteCharacters)
{
    std::u8string s = u8"č";
    Utf8::Iterator it {s};
    EXPECT_EQ(*it, U'č');
    ++it;
    EXPECT_TRUE(it == Utf8::Sentinel());
}

TEST(Utf8_Iterator, ThreeByteCharacters)
{
    // U+20AC = € = 0xE2 0x82 0xAC
    std::u8string s = u8"€";
    Utf8::Iterator it {s};
    EXPECT_EQ(*it, U'€');
    ++it;
    EXPECT_TRUE(it == Utf8::Sentinel());
}

TEST(Utf8_Iterator, SurrogateInUtf8Throws)
{
    // U+D800 encoded in UTF-8 (invalid): 0xED 0xA0 0x80
    EXPECT_ANY_THROW(Utf32::FromUtf8("\xED\xA0\x80"));
}

TEST(Utf8_Iterator, OverlongEncodingThrows)
{
    // Overlong encoding of 'A' (U+0041) as 2-byte sequence: 0xC1 0x81
    // Decoder should detect m_code < 0x80 and throw "Overlong coding in UTF-8 sequence"
    EXPECT_ANY_THROW(Utf32::FromUtf8("\xC1\x81"));
}

TEST(Utf8_Count, CountCharacters)
{
    // 1 + 1 + 1 + 1 = 4 codepoints
    EXPECT_EQ(Utf8::Count(u8"Ač€Z"), 4);
}

TEST(Utf8_FromUtf16, BasicConversion)
{
    std::u16string src = u"Hello č €";
    std::string out = Utf8::FromUtf16(src);

    // Re-decode and compare codepoints
    std::u32string decoded;
    for (Utf8::Iterator it{out}; it != Utf8::Sentinel(); ++it)
    {
        decoded.push_back(*it);
    }

    std::u32string expected = U"Hello č €";
    EXPECT_EQ(decoded, expected);
}

TEST(Utf8_FromUtf32, BasicConversion)
{
    std::u32string src = U"ABCč€";
    std::string out = Utf8::FromUtf32(src);

    std::u32string decoded;
    for (Utf8::Iterator it{out}; it != Utf8::Sentinel(); ++it)
    {
        decoded.push_back(*it);
    }

    EXPECT_EQ(decoded, src);
}

TEST(Utf8_FromUtf8, IdentityConversion)
{
    std::u8string_view src = u8"Hello č €";
    std::string out = Utf8::FromUtf8(src);

    EXPECT_EQ(out, reinterpret_cast<char const*>(src.data()));
}

TEST(Utf16_Iterator, BMPCharacters)
{
    std::u16string s = u"ABC";
    Utf16::Iterator it {s};

    EXPECT_EQ(*it, U'A');
    ++it;
    EXPECT_EQ(*it, U'B');
    ++it;
    EXPECT_EQ(*it, U'C');
    ++it;
    EXPECT_TRUE(it == Utf16::Sentinel());
}

TEST(Utf16_Iterator, SurrogatePair)
{
    // U+1F600 = surrogate pair D83D DE00
    std::u16string s = u"\U0001F600";
    Utf16::Iterator it {s};

    EXPECT_EQ(*it, 0x1F600);
    ++it;
    EXPECT_TRUE(it == Utf16::Sentinel());
}

TEST(Utf16_Count, MixedString)
{
    EXPECT_EQ(Utf16::Count(u"A\u010D\U0001F600Z"), 4);
}

TEST(Utf16_FromUtf8, BasicConversion)
{
    std::u16string out = Utf16::FromUtf8(u8"Hello č €");

    std::u32string decoded;
    for (Utf16::Iterator it{out}; it != Utf16::Sentinel(); ++it)
    {
        decoded.push_back(*it);
    }

    EXPECT_EQ(decoded, U"Hello č €");
}

TEST(Utf16_FromUtf32, BasicConversion)
{
    std::u32string src = U"ABCč€";
    std::u16string out = Utf16::FromUtf32(src);

    std::u32string decoded;
    for (Utf16::Iterator it{out}; it != Utf16::Sentinel(); ++it)
    {
        decoded.push_back(*it);
    }

    EXPECT_EQ(decoded, src);
}

TEST(Utf32_Count, CountCharacters)
{
    EXPECT_EQ(Utf32::Count(U"ABCč€"), 5);
}

TEST(Utf32_FromUtf8, BasicConversion)
{
    EXPECT_EQ(Utf32::FromUtf8(u8"ABCč€"), U"ABCč€");
}

TEST(Utf32_FromUtf16, BasicConversion)
{
    EXPECT_EQ(Utf32::FromUtf16(u"ABCč€"), U"ABCč€");
}