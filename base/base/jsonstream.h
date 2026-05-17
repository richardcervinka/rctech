#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace Rc::Json
{
    struct BeginJsonTag {};
    struct EndJsonTag {};
    struct BeginObjectTag {};
    struct EndObjectTag {};
    struct BeginArrayTag {};
    struct EndArrayTag {};
    struct TrueTag {};
    struct FalseTag {};
    struct NullTag {};

    inline constexpr BeginJsonTag BeginJson{};
    inline constexpr EndJsonTag EndJson{};
    inline constexpr BeginObjectTag BeginObject{};
    inline constexpr EndObjectTag EndObject{};
    inline constexpr BeginArrayTag BeginArray{};
    inline constexpr EndArrayTag EndArray{};
    inline constexpr TrueTag True{};
    inline constexpr FalseTag False{};
    inline constexpr NullTag Null{};

    struct Key
    {
        std::string_view name;
    };

    template<typename T>
    struct Number
    {
        T value {};
    };

    struct String
    {
        std::string_view str;
    };

    // Valid json string
    struct RawString
    {
        std::string_view str;
    };

    class Stream
    {
    public:
        explicit Stream(std::string& dst) : m_dst{dst} {}

        Stream& operator<<(BeginJsonTag);
        Stream& operator<<(EndJsonTag);
        Stream& operator<<(BeginObjectTag);
        Stream& operator<<(EndObjectTag);
        Stream& operator<<(BeginArrayTag);
        Stream& operator<<(EndArrayTag);
        Stream& operator<<(Key const& key);
        Stream& operator<<(Number<short> value);
        Stream& operator<<(Number<unsigned short> value);
        Stream& operator<<(Number<int> value);
        Stream& operator<<(Number<unsigned int> value);
        Stream& operator<<(Number<long> value);
        Stream& operator<<(Number<unsigned long> value);
        Stream& operator<<(Number<long long> value);
        Stream& operator<<(Number<unsigned long long> value);
        Stream& operator<<(Number<float> value);
        Stream& operator<<(Number<double> value);
        Stream& operator<<(TrueTag);
        Stream& operator<<(FalseTag);
        Stream& operator<<(NullTag);
        Stream& operator<<(String value);
        Stream& operator<<(RawString value);
        
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
            Initial,
            Empty,  // Created complex object
            Key,  // Key written
            Value  // Value written
        };

        template<typename T>
        Stream& Append(Number<T> number);

        std::string& m_dst;
        State m_state {State::Initial};

#ifndef NDEBUG
        std::vector<Scope> m_scope;

        Scope GetScope() const
        {
            return m_scope.empty() ? Scope::Empty : m_scope.back();
        }
#endif

    };

} // Rc::Json

inline Rc::Json::Stream& operator<<(std::string& lhs, Rc::Json::BeginJsonTag)
{
    return Rc::Json::Stream(lhs) << Rc::Json::BeginJsonTag{};
}