#pragma once

#include <string_view>
#include <stdexcept>
#include <expected>
#include <optional>
#include <functional>
#include <map>

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

    // Use this method to imlement application-specific cmd args parser (see test-cmdargs)
    std::expected<std::pair<std::string_view, std::optional<std::string_view>>, Error> Parse(std::string_view arg);

    // Map cmd line arguments to a generic map.
    std::map<std::string, std::string> Map(int argc, char* argv[]);

} // Rc::Cli