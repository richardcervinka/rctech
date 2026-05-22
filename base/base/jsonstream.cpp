#include "jsonstream.h"
#include <cassert>
#include "base/utf.h"
#include "base/debug.h"
#include <charconv>
#include <array>

namespace Rc::Json
{
    static const std::array<std::string_view, 32> escapes
    {
        "\\u0000", // 0x00
        "\\u0001", // 0x01
        "\\u0002", // 0x02
        "\\u0003", // 0x03
        "\\u0004", // 0x04
        "\\u0005", // 0x05
        "\\u0006", // 0x06
        "\\u0007", // 0x07
        "\\b",     // 0x08
        "\\t",     // 0x09
        "\\n",     // 0x0A
        "\\u000B", // 0x0B
        "\\f",     // 0x0C
        "\\r",     // 0x0D
        "\\u000E", // 0x0E
        "\\u000F", // 0x0F
        "\\u0010", // 0x10
        "\\u0011", // 0x11
        "\\u0012", // 0x12
        "\\u0013", // 0x13
        "\\u0014", // 0x14
        "\\u0015", // 0x15
        "\\u0016", // 0x16
        "\\u0017", // 0x17
        "\\u0018", // 0x18
        "\\u0019", // 0x19
        "\\u001A", // 0x1A
        "\\u001B", // 0x1B
        "\\u001C", // 0x1C
        "\\u001D", // 0x1D
        "\\u001E", // 0x1E
        "\\u001F"  // 0x1F
    };

    void Stream::AssertInitial() const
    {
        assert(m_scope.empty());
        assert(m_state == State::Initial);
    }

    void Stream::AssertState(State first, State second) const
    {
        assert((m_state == first) || (m_state == second));
    }

    void Stream::AssertKey() const
    {
        if constexpr (Rc::debug)
        {
            if (!m_scope.empty() && (m_scope.back() == Scope::Object))
            {
                assert(m_state == State::Key);
            }
        }
    }

    void Stream::AssertScope(Scope scope) const
    {
        assert(!m_scope.empty() && (m_scope.back() == scope));
    }

    void Stream::AssertScope() const
    {
        assert(m_scope.empty());
    }

    void Stream::EnterScope(Scope scope)
    {
        if constexpr (Rc::debug)
        {
            m_scope.push_back(scope);
        }
    }

    void Stream::LeaveScope()
    {
        if constexpr (Rc::debug)
        {
            m_scope.pop_back();
        }
    }

    Stream& Stream::operator<<(BeginJsonTag)
    {
        AssertInitial();

        return *this;
    }

    Stream& Stream::operator<<(EndJsonTag)
    {
        // assert(m_state ==
        AssertScope();

        return *this;
    }

    Stream& Stream::operator<<(BeginObjectTag)
    {
        AssertKey();

        if (m_state == State::Value)
        {
            m_dst += ',';
        }

        m_dst += '{';
        m_state = State::Empty;

        EnterScope(Scope::Object);

        return *this;
    }

    Stream& Stream::operator<<(EndObjectTag)
    {
        AssertScope(Scope::Object);
        AssertState(State::Empty, State::Value);

        m_dst += '}';
        m_state = State::Value;

        LeaveScope();

        return *this;
    }

    Stream& Stream::operator<<(BeginArrayTag)
    {
        AssertKey();

        if (m_state == State::Value)
        {
            m_dst += ',';
        }

        m_dst += '[';
        m_state = State::Empty;

        EnterScope(Scope::Array);

        return *this;
    }

    Stream& Stream::operator<<(EndArrayTag)
    {
        AssertScope(Scope::Array);
        AssertState(State::Empty, State::Value);

        m_dst += ']';
        m_state = State::Value;

        LeaveScope();

        return *this;
    }

    Stream& Stream::operator<<(Key const& key)
    {
        AssertScope(Scope::Object);
        AssertState(State::Empty, State::Value);

        if (m_state == State::Value)
        {
            m_dst += ',';
        }

        m_dst += '"';
        m_dst += key.name;
        m_dst += '"';
        m_dst += ':';

        m_state = State::Key;

        return *this;
    }

    Stream& Stream::operator<<(TrueTag)
    {
        AssertKey();

        if (m_state == State::Value)
        {
            m_dst += ',';
        }

        m_dst += "true";
        m_state = State::Value;

        return *this;
    }

    Stream& Stream::operator<<(FalseTag)
    {
        AssertKey();

        if (m_state == State::Value)
        {
            m_dst += ',';
        }

        m_dst += "false";
        m_state = State::Value;

        return *this;
    }

    Stream& Stream::operator<<(NullTag)
    {
        AssertKey();

        if (m_state == State::Value)
        {
            m_dst += ',';
        }

        m_dst += "null";
        m_state = State::Value;

        return *this;
    }

    Stream& Stream::operator<<(Chars value)
    {
        return *this << Chars8{{reinterpret_cast<char8_t const*>(value.str.data()), value.str.size()}};
    }
    
    Stream& Stream::operator<<(Chars8 value)
    {
        AssertKey();

        if (m_state == State::Value)
        {
            m_dst += ',';
        }

        m_dst.reserve(m_dst.size() + value.str.size() + 2u);
        m_dst += '"';

        for (auto ch : value.str)
        {
            if (static_cast<uint32_t>(ch) < 32u)
            {
                m_dst += escapes[static_cast<int>(ch)];
            }
            else if (ch == '"')
            {
                m_dst += "\\\"";
            }
            else if (ch == '\\')
            {
                m_dst += "\\\\";
            }
            else
            {
                m_dst += ch;
            }
        }

        m_dst += '"';

        m_state = State::Value;

        return *this;
    }

    Stream& Stream::operator<<(Chars16 value)
    {
        AssertKey();

        if (m_state == State::Value)
        {
            m_dst += ',';
        }

        m_dst.reserve(m_dst.size() + value.str.size() + 2u);
        m_dst += '"';

        for (Utf16::Iterator it{value.str}; it != Utf16::Sentinel(); ++it)
        {
            if (*it < 32u)
            {
                m_dst += escapes[*it];
            }
            else if (*it == U'"')
            {
                m_dst += "\\\"";
            }
            else if (*it == U'\\')
            {
                m_dst += "\\\\";
            }
            else
            {
                Utf8::PushBack(*it, m_dst);
            }
        }

        m_dst += '"';

        m_state = State::Value;

        return *this;
    }

    Stream& Stream::operator<<(Chars32 value)
    {
        AssertKey();

        if (m_state == State::Value)
        {
            m_dst += ',';
        }

        m_dst.reserve(m_dst.size() + value.str.size() + 2u);
        m_dst += '"';

        for (auto ch : value.str)
        {
            if (ch < 32u)
            {
                m_dst += escapes[ch];
            }
            else if (ch == U'"')
            {
                m_dst += "\\\"";
            }
            else if (ch == U'\\')
            {
                m_dst += "\\\\";
            }
            else
            {
                Utf8::PushBack(ch, m_dst);
            }
        }

        m_dst += '"';

        m_state = State::Value;

        return *this;
    }

    Stream& Stream::operator<<(String value)
    {
        AssertKey();

        if (m_state == State::Value)
        {
            m_dst += ',';
        }

        m_dst.reserve(m_dst.size() + value.str.size() + 2u);
        m_dst += '"';
        m_dst += value.str;
        m_dst += '"';

        m_state = State::Value;

        return *this;
    }
    
    template<typename T>
    Stream& Stream::Append(Number<T> number)
    {
        AssertKey();

        std::array<char, 64> buffer;
        const auto str = std::to_chars(buffer.data(), buffer.data() + buffer.size(), number.value);
        assert(str.ec == std::errc());

        if (m_state == State::Value)
        {
            m_dst += ',';
        }

        m_dst.append(buffer.data(), str.ptr);
        m_state = State::Value;

        return *this;
    }

    Stream& Stream::operator<<(Number<short> value)
    {
        return Append(value);
    }

    Stream& Stream::operator<<(Number<unsigned short> value)
    {
        return Append(value);
    }

    Stream& Stream::operator<<(Number<int> value)
    {
        return Append(value);
    }

    Stream& Stream::operator<<(Number<unsigned int> value)
    {
        return Append(value);
    }

    Stream& Stream::operator<<(Number<long> value)
    {
        return Append(value);
    }

    Stream& Stream::operator<<(Number<unsigned long> value)
    {
        return Append(value);
    }

    Stream& Stream::operator<<(Number<long long> value)
    {
        return Append(value);
    }

    Stream& Stream::operator<<(Number<unsigned long long> value)
    {
        return Append(value);
    }

    Stream& Stream::operator<<(Number<float> value)
    {
        return Append(value);
    }

    Stream& Stream::operator<<(Number<double> value)
    {
        return Append(value);
    }

} // Rc::Json