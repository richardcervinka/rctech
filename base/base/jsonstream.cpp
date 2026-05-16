#include "jsonstream.h"
#include <cassert>
#include "base/debug.h"
#include <charconv>
#include <array>

namespace Rc::Json
{
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

    Stream& Stream::operator<<(long long value)
    {
        //assert(m_state != State::Value);

        std::array<char, 32> buffer;
        const auto str = std::to_chars(buffer.data(), buffer.data() + buffer.size(), value);
        assert(str.ec == std::errc());

        if (m_state == State::Value)
        {
            m_dst += ',';
        }

        m_dst += std::string_view{buffer.data(), str.ptr};
        m_state = State::Value;

        return *this;
    }

    Stream& Stream::operator<<(unsigned long long value)
    {
        // assert(!m_state.empty());
        // assert(m_state.back() == State::Object);

        std::array<char, 32> buffer;
        const auto str = std::to_chars(buffer.data(), buffer.data() + buffer.size(), value);
        assert(str.ec == std::errc());

        if (m_state == State::Value)
        {
            m_dst += ',';
        }

        m_dst += std::string_view{buffer.data(), str.ptr};
        m_state = State::Value;

        return *this;
    }

    Stream& Stream::operator<<(float value)
    {
        // assert(!m_state.empty());
        // assert(m_state.back() == State::Object);

        // m_dst += Str::From(value);

        // m_state.back() = State::Value;

        if (m_state == State::Value)
        {
            m_dst += ',';
        }

        m_dst += '0';
        m_state = State::Value;

        return *this;
    }

    Stream& Stream::operator<<(double value)
    {
        // assert(!m_state.empty());
        // assert(m_state.back() == State::Object);

        // m_dst += Str::From(value);

        // m_state.back() = State::Value;

        if (m_state == State::Value)
        {
            m_dst += ',';
        }

        m_dst += '0';
        m_state = State::Value;

        return *this;
    }

    Stream& Stream::operator<<(bool value)
    {
        if (m_state == State::Value)
        {
            m_dst += ',';
        }

        m_dst += value ? "true" : "false";
        m_state = State::Value;

        return *this;
    }

    Stream& Stream::operator<<(nullptr_t value)
    {
        if (m_state == State::Value)
        {
            m_dst += ',';
        }

        m_dst += "null";
        m_state = State::Value;

        return *this;
    }

} // Rc::Json