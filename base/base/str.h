#pragma once

#include <string>
#include <string_view>
#include <expected>
#include <array>
#include <format>
#include <charconv>

namespace Rc::Str
{
    template<typename T>
    std::expected<T, std::errc> To(std::string_view str)
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

    template<typename T>
    std::string From(T value)
    {
        std::array<char, 64> buffer;
        const auto result = std::to_chars(buffer.data(), buffer.data() + buffer.size(), value);

        assert(result.ec == std::errc());

        return {buffer.data(), result.ptr};
    }

    template<>
    inline std::expected<bool, std::errc> To(std::string_view str)
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

    template<>
    inline std::string From(bool value)
    {
        return value ? "true" : "false";
    }

    template<typename T>
    struct Formatter : std::formatter<std::string_view>
    {
        auto format(T value, std::format_context& ctx) const
        {
            return std::formatter<std::string_view>::format(Rc::Str::From(value), ctx);
        }
    };

} // Rc::Str