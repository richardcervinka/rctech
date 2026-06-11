#pragma once

#include "base/str.h"
#include <compare>

namespace Rc::Render
{
    struct Version
    {
        int major {0};
        int minor {0};

        auto operator<=>(Version const&) const = default;
    };

} // Rc::Render

namespace Rc::Str
{
    inline std::string From(Rc::Render::Version value)
    {
        return std::format("{}.{}", value.major, value.minor);
    }
    
} // Rc::Str