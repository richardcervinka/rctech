#pragma once

#include <string>
#include <string_view>
#include <iterator>

namespace Rc::Utf8
{
    using Sentinel = std::default_sentinel_t;

    class Iterator
    {
    public:
        explicit Iterator(std::string_view src) :
            src{reinterpret_cast<char8_t const*>(std::to_address(src.cbegin())), src.size()}
        {}

        explicit Iterator(std::u8string_view src) : src{src} {}

        char32_t operator*() const;
        Iterator& operator++();
        bool operator==(Sentinel) const noexcept;
        bool operator!=(Sentinel) const noexcept;

    private:
        void Decode() const;

        std::u8string_view src;
        mutable char32_t code {0};
        mutable int step {0};
    };

    std::size_t PushBack(char32_t ch, std::string& dst);

    // Encoding conversion.
    std::string FromUtf8(std::u8string_view src);
    std::string FromUtf8(std::u8string_view src, std::string&& buffer);
    std::string FromUtf16(std::u16string_view src);
    std::string FromUtf16(std::u16string_view src, std::string&& buffer);
    std::string FromUtf32(std::u32string_view src);
    std::string FromUtf32(std::u32string_view src, std::string&& buffer);

    // Number of characters.
    std::size_t Count(std::string_view src);
    std::size_t Count(std::u8string_view src);

} // Rc::Utf8

namespace Rc::Utf16
{
    using Sentinel = std::default_sentinel_t;

    class Iterator
    {
    public:
        explicit Iterator(std::u16string_view src) : src{src} {}

        char32_t operator*() const;
        Iterator& operator++();
        bool operator==(Sentinel) const noexcept;
        bool operator!=(Sentinel) const noexcept;

    private:
        void Decode() const;

        std::u16string_view src;
        mutable char32_t code {0};
        mutable int step {0};
    };

    std::size_t PushBack(char32_t ch, std::u16string& dst);

    // Encoding conversion.
    std::u16string FromUtf8(std::string_view src);
    std::u16string FromUtf8(std::string_view src, std::u16string&& buffer);
    std::u16string FromUtf8(std::u8string_view src);
    std::u16string FromUtf8(std::u8string_view src, std::u16string&& buffer);
    std::u16string FromUtf32(std::u32string_view src);
    std::u16string FromUtf32(std::u32string_view src, std::u16string&& buffer);

    // Number of characters.
    std::size_t Count(std::u16string_view src);

} // Rc::Utf16

namespace Rc::Utf32
{
    // Encoding conversion.
    std::u32string FromUtf8(std::string_view src);
    std::u32string FromUtf8(std::string_view src, std::u32string&& buffer);
    std::u32string FromUtf8(std::u8string_view src);
    std::u32string FromUtf8(std::u8string_view, std::u32string&& buffer);
    std::u32string FromUtf16(std::u16string_view src);
    std::u32string FromUtf16(std::u16string_view src, std::u32string&& buffer);

    // Number of characters.
    std::size_t Count(std::u32string_view src);

} // Rc::Utf32