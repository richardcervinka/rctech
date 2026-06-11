#include "str.h"
#include <array>

namespace Rc::Str
{
    static inline std::string FromNumeric(auto value)
    {
        std::array<char, 64> buffer;
        const auto result = std::to_chars(buffer.data(), buffer.data() + buffer.size(), value);

        assert(result.ec == std::errc());

        return {buffer.data(), result.ptr};
    }

    std::string From(int v)
    {
        return FromNumeric(v);
    }

    std::string From(unsigned v)
    {
        return FromNumeric(v);
    }

    std::string From(long v)
    {
        return FromNumeric(v);
    }

    std::string From(unsigned long v)
    {
        return FromNumeric(v);
    }

    std::string From(long long v)
    {
        return FromNumeric(v);
    }
    
    std::string From(unsigned long long v)
    {
        return FromNumeric(v);
    }

    std::string From(float v)
    {
        return FromNumeric(v);
    }

    std::string From(double v)
    {
        return FromNumeric(v);
    }

    std::string From(bool value)
    {
        return value ? "true" : "false";
    }

    template<typename T>
    std::expected<T, std::errc> ToNumeric(std::string_view str)
    {
        T result {};
        auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), result);

        if (ptr != (str.data() + str.size()))
        {
            return std::unexpected{std::errc::invalid_argument};
        }
        if (ec == std::errc())
        {
            return result;
        }
        return std::unexpected{ec};
    }

    template<>
    std::expected<int, std::errc> To<int>(std::string_view str)
    {
        return ToNumeric<int>(str);
    }

    template<>
    std::expected<unsigned, std::errc> To<unsigned>(std::string_view str)
    {
        return ToNumeric<unsigned>(str);
    }

    template<>
    std::expected<long, std::errc> To<long>(std::string_view str)
    {
        return ToNumeric<long>(str);
    }

    template<>
    std::expected<unsigned long, std::errc> To<unsigned long>(std::string_view str)
    {
        return ToNumeric<unsigned long>(str);
    }

    template<>
    std::expected<long long, std::errc> To<long long>(std::string_view str)
    {
        return ToNumeric<long long>(str);
    }

    template<>
    std::expected<unsigned long long, std::errc> To<unsigned long long>(std::string_view str)
    {
        return ToNumeric<unsigned long long>(str);
    }

    template<>
    std::expected<float, std::errc> To<float>(std::string_view str)
    {
        return ToNumeric<float>(str);
    }

    template<>
    std::expected<double, std::errc> To<double>(std::string_view str)
    {
        return ToNumeric<double>(str);
    }

    template<>
    std::expected<bool, std::errc> To<bool>(std::string_view str)
    {
        if (str == "true")
        {
            return true;
        }
        if (str == "false")
        {
            return false;
        }
        return std::unexpected{std::errc::invalid_argument};
    }
    
} // Rc::Str