#pragma once

#include <string_view>
#include <stdexcept>
#include <expected>
#include <optional>
#include <functional>
#include <map>
#include <cassert>
#include <format>
#include <ostream>
#include "str.h"

// Command Line Interface
namespace Rc::Cli
{
    enum class Error
    {
        BadPrefix,
        BadName,
        BadValue,
        BadFormat
    };

    // Use this method to implement application-specific cmd args parser (see test-cli)
    std::expected<std::pair<std::string_view, std::optional<std::string_view>>, Error> Parse(std::string_view arg);

    // Map cmd line arguments to a generic map.
    std::map<std::string, std::string> Map(int argc, char* argv[]);

} // Rc::Cli

namespace Rc::Str
{
    template<>
    std::expected<Rc::Cli::Error, std::errc> To(std::string_view str);

    template<>
    std::string From(Rc::Cli::Error value);

} // Rc::Str
