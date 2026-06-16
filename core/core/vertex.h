#pragma once

#include <cstdint>
#include <array>
#include "base/float.h"

namespace Rc::Gfx
{
    enum class VertexAttribute
    {
        // VK_FORMAT_R32G32B32_SFLOAT
        Position,

        // VK_FORMAT_R32G32B32_SFLOAT
        Normal,

        // VK_FORMAT_R32G32B32_SFLOAT
        Color
    };

    struct VertexDescription
    {
        VertexAttribute attribute;
        uint32_t offset;
    };

    struct VertexBasic
    {
        Float3 position;
        Float3 color;

        // inline static const uint32_t stride
        // {
        //     sizeof(VertexBasic::position) +
        //     sizeof(VertexBasic::color)
        // };

        inline static const std::array<VertexDescription, 2> attributes
        {
            VertexDescription{VertexAttribute::Position, 0},
            VertexDescription{VertexAttribute::Color, 12}
        };
    };

} // Rc::Gfx