#include "jsonstream.h"
#include <cassert>
#include "base/utf.h"
#include "base/debug.h"
#include <charconv>
#include <array>

namespace Rc::Json
{
    static constexpr std::array<std::string_view, 32> escapes
    {
        "\\u0000",
        "\\u0001",
        "\\u0002",
        "\\u0003",
        "\\u0004",
        "\\u0005",
        "\\u0006",
        "\\u0007",
        "\\b",
        "\\t",
        "\\n",
        "\\u000B",
        "\\f",
        "\\r",
        "\\u000E",
        "\\u000F",
        "\\u0010",
        "\\u0011",
        "\\u0012",
        "\\u0013",
        "\\u0014",
        "\\u0015",
        "\\u0016",
        "\\u0017",
        "\\u0018",
        "\\u0019",
        "\\u001A",
        "\\u001B",
        "\\u001C",
        "\\u001D",
        "\\u001E",
        "\\u001F"
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
                m_dst += static_cast<char>(ch);
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