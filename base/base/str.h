#pragma once

#include <string>
#include <string_view>
#include <expected>
#include <charconv>
#include <cassert>

namespace Rc::Str
{
    std::string From(int v);
    std::string From(unsigned v);
    std::string From(long v);
    std::string From(unsigned long v);
    std::string From(long long v);
    std::string From(unsigned long long v);
    std::string From(float v);
    std::string From(double v);
    std::string From(bool value);

    template<typename T>
    std::expected<T, std::errc> To(std::string_view str);

    template<> std::expected<int, std::errc> To<int>(std::string_view str);
    template<> std::expected<unsigned, std::errc> To<unsigned>(std::string_view str);
    template<> std::expected<long, std::errc> To<long>(std::string_view str);
    template<> std::expected<unsigned long, std::errc> To<unsigned long>(std::string_view str);
    template<> std::expected<long long, std::errc> To<long long>(std::string_view str);
    template<> std::expected<unsigned long long, std::errc> To<unsigned long long>(std::string_view str);
    template<> std::expected<float, std::errc> To<float>(std::string_view str);
    template<> std::expected<double, std::errc> To<double>(std::string_view str);
    template<> std::expected<bool, std::errc> To<bool>(std::string_view str);

} // Rc::Str