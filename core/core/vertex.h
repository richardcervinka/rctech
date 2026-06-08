#pragma once

#include <cstdint>
#include "base/float.h"

namespace Rc::Gfx
{
    struct VertexBasic
    {
        Float3 position;
        Float3 color;

        inline static const uint32_t stride
        {
            sizeof(VertexBasic::position) +
            sizeof(VertexBasic::color)
        };
    };

} // Rc::Gfx