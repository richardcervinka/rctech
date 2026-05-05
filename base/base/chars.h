#pragma once

#include <string>
#include <string_view>
#include <expected>
#include <array>

namespace Rc::Chars
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

    template<>
    std::expected<bool, std::errc> To(std::string_view str)
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

    template<typename T>
    std::string From(T value)
    {
        std::array<char, 64> buffer;
        const auto result = std::to_chars(buffer.data(), buffer.data() + buffer.size(), value);

        assert(result.ec == std::errc());

        return {buffer.data(), result.ptr};
    }
    
} // Rc::String