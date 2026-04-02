#pragma once

#include "base/float.h"

namespace Rc
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

} // Rc