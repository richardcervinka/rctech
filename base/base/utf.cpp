#include "utf.h"
#include <cassert>
#include <stdexcept>

static_assert(sizeof("ě") == 3, "String literals are not UTF-8!");

namespace Rc::Utf8
{
    void Iterator::Decode() const
    {
        if (m_src.empty())
        {
            throw std::runtime_error("Empty UTF-8 sequence");
        }

        // 1 byte character 0xxxxxxx
        if (m_src[0] < 0x80u)
        {
            m_code = static_cast<char32_t>(m_src[0]);
            m_step = 1;
            return;
        }

        // 2 byte character 110xxxxx 10xxxxxx
        if (m_src[0] < 0xE0u)
        {
            if (m_src.size() < 2)
            {
                throw std::runtime_error("Bad UTF-8 sequence");
            }
            if ((m_src[1] & 0b11000000u) != 0b10000000u)
            {
                throw std::runtime_error("Bad UTF-8 sequence");
            }

            m_code = {
                ((m_src[0] & 0x01Fu) << 6) |
                ((m_src[1] & 0x03Fu) << 0)
            };
            
            if (m_code < 0x80u)
            {
                throw std::runtime_error("Overlong coding in UTF-8 sequence");
            }

            m_step = 2;
            return;
        }

        // 3 byte character 1110xxxx 10xxxxxx 10xxxxxx
        if (m_src[0] < 0xF0u)
        {
            if (m_src.size() < 3)
            {
                throw std::runtime_error("Bad UTF-8 sequence");
            }
            if ((m_src[1] & 0b11000000u) != 0b10000000u)
            {
                throw std::runtime_error("Bad UTF-8 sequence");
            }
            if ((m_src[2] & 0b11000000u) != 0b10000000u)
            {
                throw std::runtime_error("Bad UTF-8 sequence");
            }

            m_code = {
                ((m_src[0] & 0x000Fu) << 12) |
                ((m_src[1] & 0x003Fu) << 6) |
                ((m_src[2] & 0x003Fu) << 0)
            };

            // Surrogates (U+D800 - U+DFFF)
            if ((m_code >= 0xD800u) && (m_code <= 0xDFFFu))
            {
                throw std::runtime_error("Surrogates in UTF-8 sequence");
            }
            if (m_code < 0x800u)
            {
                throw std::runtime_error("Overlong coding in UTF-8 sequence");
            }

            m_step = 3;
            return;
        }

        // 4 byte character 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
        if (m_src[0] < 0xF8u)
        {
            if (m_src.size() < 4)
            {
                throw std::runtime_error("Bad UTF-8 sequence");
            }

            if ((m_src[1] & 0b11000000u) != 0b10000000u)
            {
                throw std::runtime_error("Bad UTF-8 sequence");
            }
            if ((m_src[2] & 0b11000000u) != 0b10000000u)
            {
                throw std::runtime_error("Bad UTF-8 sequence");
            }
            if ((m_src[3] & 0b11000000u) != 0b10000000u)
            {
                throw std::runtime_error("Bad UTF-8 sequence");
            }

            m_code = {
                ((m_src[0] & 0x0007u) << 18) |
                ((m_src[1] & 0x003Fu) << 12) |
                ((m_src[2] & 0x003Fu) << 6) |
                ((m_src[3] & 0x003Fu) << 0)
            };

            if (m_code > 0x10FFFFu)
            {
                throw std::runtime_error("UTF-8 character is out of range 0x10FFFF");
            }
            if (m_code < 0x10000u)
            {
                throw std::runtime_error("Overlong coding in UTF-8 sequence");
            }

            m_step = 4;
            return;
        }

        throw std::runtime_error("Bad UTF-8 sequence");
    }

    char32_t Iterator::operator*() const
    {
        if (m_step == 0)
        {
            Decode();
        }

        return m_code;
    }

    Iterator& Iterator::operator++()
    {
        if (m_step == 0)
        {
            Decode();
        }
        
        m_src = m_src.substr(m_step);
        m_step = 0;
        return *this;
    }

    bool Iterator::operator==(Sentinel) const noexcept
    {
        return m_src.empty();
    }

    bool Iterator::operator!=(Sentinel) const noexcept
    {
        return !m_src.empty();
    }

} // Rc::Utf8

namespace Rc::Utf16
{
    void Iterator::Decode() const
    {
        if (m_src.empty())
        {
            throw std::runtime_error("Empty UTF-16 sequence");
        }

        // Surrogate pairs.
        if ((m_src[0] >= 0xD800u) && (m_src[0] <= 0xDFFFu))
        {
            if (m_src.size() < 2)
            {
                throw std::runtime_error("Bad size of UTF-16 sequence");
            }

            m_code = ((m_src[0] & 0x3FFu) << 10) | (m_src[1] & 0x3FFu);
            m_code += 0x010000u;
            m_step = 2;
            return;
        }

        // One char16 character.
        m_code = m_src[0];
        m_step = 1;
    }

    char32_t Iterator::operator*() const
    {
        if (m_step == 0)
        {
            Decode();
        }
        
        return m_code;
    }

    Iterator& Iterator::operator++()
    {
        if (m_step == 0)
        {
            Decode();
        }

        m_src = m_src.substr(m_step);
        m_step = 0;
        return *this;
    }

    bool Iterator::operator==(Sentinel) const noexcept
    {
        return m_src.empty();
    }

    bool Iterator::operator!=(Sentinel) const noexcept
    {
        return !m_src.empty();
    }

} // Rc

namespace Rc::Utf8
{
    std::size_t Count(std::string_view src)
    {
        std::size_t count = 0;
        for (Utf8::Iterator it{src}; it != Utf8::Sentinel(); ++it)
        {
            count++;
        }
        return count;
    }

    std::size_t Count(std::u8string_view src)
    {
        std::size_t count = 0;
        for (Utf8::Iterator it{src}; it != Utf8::Sentinel(); ++it)
        {
            count++;
        }
        return count;
    }

    std::size_t PushBack(char32_t ch, std::string& dst)
    {
        // 1 byte
        if (ch <= 0x7Fu)
        {
            dst.push_back(static_cast<char>(ch));
            return 1u;
        }
        // 2 byte
        if (ch <= 0x7FFu)
        {
            dst.push_back(static_cast<char>(0xC0u | ((ch >> 6) & 0x1Fu)));
            dst.push_back(static_cast<char>(0x80u | ((ch >> 0) & 0x3Fu)));
            return 2u;
        }
        // 3 byte
        if (ch <= 0xFFFFu)
        {
            dst.push_back(static_cast<char>(0xE0u | ((ch >> 12) & 0x0Fu)));
            dst.push_back(static_cast<char>(0x80u | ((ch >> 6) & 0x3Fu)));
            dst.push_back(static_cast<char>(0x80u | ((ch >> 0) & 0x3Fu)));
            return 3u;
        }
        // 4 byte
        if (ch <= 0x1FFFFFu)
        {
            dst.push_back(static_cast<char>(0xF0u | ((ch >> 18) & 0x07u)));
            dst.push_back(static_cast<char>(0x80u | ((ch >> 12) & 0x3Fu)));
            dst.push_back(static_cast<char>(0x80u | ((ch >> 6) & 0x3Fu)));
            dst.push_back(static_cast<char>(0x80u | ((ch >> 0) & 0x3Fu)));
            return 4u;
        }

        throw std::runtime_error("Bad UTF-8 character");
    }

    std::string FromUtf8(std::u8string_view src)
    {
        return std::string(reinterpret_cast<char const*>(src.data()), src.size());
    }

    std::string FromUtf8(std::u8string_view src, std::string&& buffer)
    {
        buffer.append(reinterpret_cast<char const*>(src.data()), src.size());
        return buffer;
    }

    std::string FromUtf16(std::u16string_view src)
    {
        std::string str;
        str.reserve(src.size());

        for (Utf16::Iterator it{src}; it != Utf16::Sentinel(); ++it)
        {
            PushBack(*it, str);
        }

        return str;
    }

    std::string FromUtf16(std::u16string_view src, std::string&& buffer)
    {
        buffer.reserve(buffer.size() + src.size());

        for (Utf16::Iterator it{src}; it != Utf16::Sentinel(); ++it)
        {
            PushBack(*it, buffer);
        }

        return buffer;
    }

    std::string FromUtf32(std::u32string_view src)
    {
        std::string str;
        str.reserve(src.size());

        for (auto ch : src)
        {
            PushBack(ch, str);
        }

        return str;
    }

    std::string FromUtf32(std::u32string_view src, std::string&& buffer)
    {
        buffer.reserve(buffer.size() + src.size());

        for (auto ch : src)
        {
            PushBack(ch, buffer);
        }

        return buffer;
    }

} // Rc::Utf8

namespace Rc::Utf16
{
    std::size_t Count(std::u16string_view src)
    {
        std::size_t count = 0;
        for (Utf16::Iterator it{src}; it != Utf16::Sentinel(); ++it)
        {
            count++;
        }
        return count;
    }

    std::size_t PushBack(char32_t ch, std::u16string& dst)
    {
        // Unicode define no characters in range 0xd800 - 0xdfff.
        if ((ch >= 0xD800u) && (ch <= 0xDFFFu))
        {
            return 0u;
        }

        // Surrogate pairs.
        if (ch > 0xFFFFu)
        {
            dst.push_back(0xD800u | (((ch - 0x010000u) >> 10) & 0x3FFu));
            dst.push_back(0xDC00u | (((ch - 0x010000u) >> 0) & 0x3FFu));
            return 2u;
        }

        // One character.
        dst.push_back(static_cast<char16_t>(ch));
        return 1u;
    }

    std::u16string FromUtf8(std::string_view src)
    {
        std::u16string str;
        str.reserve(src.size());

        for (Utf8::Iterator it{src}; it != Utf8::Sentinel(); ++it)
        {
            PushBack(*it, str);
        }

        return str;
    }

    std::u16string FromUtf8(std::string_view src, std::u16string&& buffer)
    {
        buffer.reserve(buffer.size() + src.size());

        for (Utf8::Iterator it{src}; it != Utf8::Sentinel(); ++it)
        {
            PushBack(*it, buffer);
        }

        return buffer;
    }

    std::u16string FromUtf8(std::u8string_view src)
    {
        std::u16string str;
        str.reserve(src.size());

        for (Utf8::Iterator it{src}; it != Utf8::Sentinel(); ++it)
        {
            PushBack(*it, str);
        }

        return str;
    }

    std::u16string FromUtf8(std::u8string_view src, std::u16string&& buffer)
    {
        buffer.reserve(buffer.size() + src.size());

        for (Utf8::Iterator it{src}; it != Utf8::Sentinel(); ++it)
        {
            PushBack(*it, buffer);
        }

        return buffer;
    }

    std::u16string FromUtf32(std::u32string_view src)
    {
        std::u16string str;
        str.reserve(src.size());

        for (auto ch : src)
        {
            PushBack(ch, str);
        }

        return str;
    }

    std::u16string FromUtf32(std::u32string_view src, std::u16string&& buffer)
    {
        buffer.reserve(buffer.size() + src.size());

        for (auto ch : src)
        {
            PushBack(ch, buffer);
        }

        return buffer;
    }

} // Rc::Utf16

namespace Rc::Utf32
{
    std::size_t Count(std::u32string_view src)
    {
        return src.size();
    }

    std::u32string FromUtf8(std::string_view src)
    {
        std::u32string str;
        str.reserve(src.size());

        for (Utf8::Iterator it{src}; it != Utf8::Sentinel(); ++it)
        {
            str.push_back(*it);
        }

        return str; 
    }

    std::u32string FromUtf8(std::string_view src, std::u32string&& buffer)
    {
        buffer.reserve(buffer.size() + src.size());

        for (Utf8::Iterator it{src}; it != Utf8::Sentinel(); ++it)
        {
            buffer.push_back(*it);
        }

        return buffer; 
    }

    std::u32string FromUtf8(std::u8string_view src)
    {
        std::u32string str;
        str.reserve(src.size());

        for (Utf8::Iterator it{src}; it != Utf8::Sentinel(); ++it)
        {
            str.push_back(*it);
        }

        return str; 
    }

    std::u32string FromUtf8(std::u8string_view src, std::u32string&& buffer)
    {
        buffer.reserve(buffer.size() + src.size());

        for (Utf8::Iterator it{src}; it != Utf8::Sentinel(); ++it)
        {
            buffer.push_back(*it);
        }

        return buffer; 
    }

    std::u32string FromUtf16(std::u16string_view src)
    {
        std::u32string str;
        str.reserve(src.size());

        for (Utf16::Iterator it{src}; it != Utf16::Sentinel(); ++it)
        {
            str.push_back(*it);
        }

        return str;
    }
    
    std::u32string FromUtf16(std::u16string_view src, std::u32string&& buffer)
    {
        buffer.reserve(buffer.size() + src.size());

        for (Utf16::Iterator it{src}; it != Utf16::Sentinel(); ++it)
        {
            buffer.push_back(*it);
        }

        return buffer;
    }
   
} // Rc