#include "jsonstream.h"
#include <cassert>
#include "base/debug.h"
#include <charconv>
#include <array>

namespace Rc::Json
{
    Stream& Stream::operator<<(BeginJsonTag)
    {
        assert(m_state == State::Initial);
        assert(m_scope.empty());

        return *this;
    }

    Stream& Stream::operator<<(EndJsonTag)
    {
        // assert(m_state ==
        assert(m_scope.empty());

        return *this;
    }

    Stream& Stream::operator<<(BeginObjectTag)
    {
        if constexpr (Rc::debug)
        {
            if (GetScope() == Scope::Object)
            {
                assert(m_state == State::Key);
            }
        }

        if (m_state == State::Value)
        {
            m_dst += ',';
        }

        m_dst += '{';
        m_state = State::Empty;

        if constexpr (Rc::debug)
        {
            m_scope.push_back(Scope::Object);
        }

        return *this;
    }

    Stream& Stream::operator<<(EndObjectTag)
    {
        assert(GetScope() == Scope::Object);

        m_dst += '}';
        m_state = State::Value;

        if constexpr (Rc::debug)
        {
            m_scope.pop_back();
        }

        return *this;
    }

    Stream& Stream::operator<<(BeginArrayTag)
    {
        if constexpr (Rc::debug)
        {
            if (GetScope() == Scope::Object)
            {
                assert(m_state == State::Key);
            }
        }

        if (m_state == State::Value)
        {
            m_dst += ',';
        }

        m_dst += '[';
        m_state = State::Empty;

        if constexpr (Rc::debug)
        {
            m_scope.push_back(Scope::Array);
        }

        return *this;
    }

    Stream& Stream::operator<<(EndArrayTag)
    {
        assert(GetScope() == Scope::Array);

        m_dst += ']';
        m_state = State::Value;

        if constexpr (Rc::debug)
        {
            m_scope.pop_back();
        }

        return *this;
    }

    Stream& Stream::operator<<(Key const& key)
    {
        assert(GetScope() == Scope::Object);
        assert(m_state != State::Key);

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
        if constexpr (Rc::debug)
        {
            if (GetScope() == Scope::Object)
            {
                assert(m_state == State::Key);
            }
        }

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
        if constexpr (Rc::debug)
        {
            if (GetScope() == Scope::Object)
            {
                assert(m_state == State::Key);
            }
        }

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
        if constexpr (Rc::debug)
        {
            if (GetScope() == Scope::Object)
            {
                assert(m_state == State::Key);
            }
        }

        if (m_state == State::Value)
        {
            m_dst += ',';
        }

        m_dst += "null";
        m_state = State::Value;

        return *this;
    }

    Stream& Stream::operator<<(String value)
    {
        if constexpr (Rc::debug)
        {
            if (GetScope() == Scope::Object)
            {
                assert(m_state == State::Key);
            }
        }

        if (m_state == State::Value)
        {
            m_dst += ',';
        }

        m_dst.reserve(m_dst.size() + value.str.size() + 2u);
        m_dst += '"';
        
        for (auto ch : value.str)
        {
            switch (ch)
            {
                case '"':
                    m_dst += "\\\"";
                    break;
                case '\\':
                    m_dst += "\\\\";
                    break;
                case '\n':
                    m_dst += "\\n";
                    break;
                case '\t':
                    m_dst += "\\t"; 
                    break;
                case '\r':
                    m_dst += "\\r";
                    break;
                default:
                    m_dst += ch;
                    break;
                // TODO: charcode < 127
            }
        }

        m_dst += '"';

        m_state = State::Value;

        return *this;
    }

    Stream& Stream::operator<<(RawString value)
    {
        if constexpr (Rc::debug)
        {
            if (GetScope() == Scope::Object)
            {
                assert(m_state == State::Key);
            }
        }

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
        if constexpr (Rc::debug)
        {
            if (GetScope() == Scope::Object)
            {
                assert(m_state == State::Key);
            }
        }

        std::array<char, 32> buffer;
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