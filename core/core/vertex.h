#pragma once

#include <cstdint>
#include <array>
#include "base/float.h"
#include "base/matrix.h"

namespace Rc::Gfx
{
    enum class VertexAttribute
    {
        // VK_FORMAT_R32G32B32_SFLOAT
        Position,

        // VK_FORMAT_R32G32B32_SFLOAT
        Normal,

        // VK_FORMAT_R32G32B32_SFLOAT
        Color,

        // VK_FORMAT_R32G32B32A32_SFLOAT
        Transformations0,
        Transformations1,
        Transformations2,
        Transformations3,
    };

    enum class VertexBinding
    {
        PerVertex,
        PerInstance
    };

    struct VertexDescription
    {
        VertexAttribute attribute;
        VertexBinding binding;
        uint32_t offset;
    };

    struct VertexBasic
    {
        Float3 position;
        Float3 color;

        inline static const std::array<VertexDescription, 2> attributes
        {
            VertexDescription
            {
                .attribute = VertexAttribute::Position,
                .binding = VertexBinding::PerVertex,
                .offset = 0
            },
            VertexDescription
            {
                .attribute = VertexAttribute::Color,
                .binding = VertexBinding::PerVertex,
                .offset = 12
            }
        };
    };

    struct VertexInstance
    {
        Matrix4<float> transformations;

        inline static const std::array<VertexDescription, 4> attributes
        {
            VertexDescription
            {
                .attribute = VertexAttribute::Transformations0,
                .binding = VertexBinding::PerInstance,
                .offset = 0
            },
            VertexDescription
            {
                .attribute = VertexAttribute::Transformations1,
                .binding = VertexBinding::PerInstance,
                .offset = 4 * sizeof(float)
            },
            VertexDescription
            {
                .attribute = VertexAttribute::Transformations2,
                .binding = VertexBinding::PerInstance,
                .offset = 8 * sizeof(float)
            },
            VertexDescription
            {
                .attribute = VertexAttribute::Transformations3,
                .binding = VertexBinding::PerInstance,
                .offset = 12 * sizeof(float)
            }
        };
    };

} // Rc::Gfx