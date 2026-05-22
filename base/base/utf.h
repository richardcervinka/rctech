#pragma once

#include <string>
#include <string_view>
#include <iterator>
#include <stdexcept>

namespace Rc::Utf8
{
    using Sentinel = std::default_sentinel_t;

    class Iterator
    {
    public:
        explicit Iterator(std::string_view src) :
            m_src{reinterpret_cast<char8_t const*>(std::to_address(src.cbegin())), src.size()}
        {}

        explicit Iterator(std::u8string_view src) : m_src{src} {}

        char32_t operator*() const;
        Iterator& operator++();
        bool operator==(Sentinel const& sentinel) const noexcept;
        bool operator!=(Sentinel const& sentinel) const noexcept;

    private:
        void Decode() const;

        std::u8string_view m_src;
        mutable char32_t m_code {0};
        mutable int m_step {0};
    };

    std::size_t PushBack(const char32_t ch, std::string& dst);

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
        explicit Iterator(std::u16string_view src) : m_src{src} {}

        const char32_t operator*() const;
        Iterator& operator++();
        bool operator==(Sentinel const& sentinel) const noexcept;
        bool operator!=(Sentinel const& sentinel) const noexcept;

    private:
        void Decode() const;

    private:
        std::u16string_view m_src;
        mutable char32_t m_code {0};
        mutable int m_step {0};
    };

    std::size_t PushBack(const char32_t ch, std::u16string& dst);

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