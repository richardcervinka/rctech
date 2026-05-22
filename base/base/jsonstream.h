#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <cassert>

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

    template<typename T>
    struct CharsView
    {
        std::basic_string_view<T> str;
    };

    // UTF-8 string
    using Chars = CharsView<char>;

    // UTF-8 string
    using Chars8 = CharsView<char8_t>;

    // UTF-16 string
    using Chars16 = CharsView<char16_t>;

    // UTF-32 string
    using Chars32 = CharsView<char32_t>;

    // Valid json string
    struct String
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
        Stream& operator<<(Chars value);
        Stream& operator<<(Chars8 value);
        Stream& operator<<(Chars16 value);
        Stream& operator<<(Chars32 value);

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
        #endif

        void AssertInitial() const;
        void AssertState(State first, State second) const;
        void AssertKey() const;
        void AssertScope(Scope scope) const;
        void AssertScope() const;
        void EnterScope(Scope scope);
        void LeaveScope();
    };

} // Rc::Json