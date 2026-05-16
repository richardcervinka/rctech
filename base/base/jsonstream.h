#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace Rc::Json
{
    struct BeginObjectTag {};
    struct EndObjectTag {};
    struct BeginArrayTag {};
    struct EndArrayTag {};

    inline constexpr BeginObjectTag BeginObject{};
    inline constexpr EndObjectTag EndObject{};
    inline constexpr BeginArrayTag BeginArray{};
    inline constexpr EndArrayTag EndArray{};

    struct Key
    {
        std::string_view name;
    };

    class Stream
    {
    public:
        explicit Stream(std::string& dst) : m_dst{dst} {}

        Stream& operator<<(BeginObjectTag);
        Stream& operator<<(EndObjectTag);
        Stream& operator<<(BeginArrayTag);
        Stream& operator<<(EndArrayTag);
        Stream& operator<<(Key const& key);

        Stream& operator<<(short value)
        {
            return *this << static_cast<long long>(value);
        }

        Stream& operator<<(unsigned short value)
        {
            return *this << static_cast<unsigned long long>(value);
        }

        Stream& operator<<(int value)
        {
            return *this << static_cast<long long>(value);
        }

        Stream& operator<<(unsigned int value)
        {
            return *this << static_cast<unsigned long long>(value);
        }

        Stream& operator<<(long value)
        {
            return *this << static_cast<long long>(value);
        }

        Stream& operator<<(unsigned long value)
        {
            return *this << static_cast<unsigned long long>(value);
        }

        Stream& operator<<(long long value);
        Stream& operator<<(unsigned long long value);
        Stream& operator<<(float value);
        Stream& operator<<(double value);
        Stream& operator<<(bool value);
        Stream& operator<<(nullptr_t value);

        // TODO: string
        // TODO: char

    private:
        enum class Scope
        {
            Empty,
            Object,
            Array
        };

        enum class State
        {
            Empty,  // Created complex object
            Key,  // Key written
            Value  // Value written
        };

        std::string& m_dst;
        State m_state {State::Empty};

#ifndef NDEBUG
        std::vector<Scope> m_scope;

        Scope GetScope() const
        {
            return m_scope.empty() ? Scope::Empty : m_scope.back();
        }
#endif

    };

} // Rc::Json