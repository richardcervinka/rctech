#pragma once

#include <compare>

namespace Rc::Render
{
    struct Version
    {
        int major;
        int minor;

        auto operator<=>(Version const&) const = default;
    };

} // Rc::Render