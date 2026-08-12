#pragma once

#include <cstdint>
#include <array>
#include <span>
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

        // Local transformations
        // VK_FORMAT_R32G32B32A32_SFLOAT
        Local0,
        Local1,
        Local2,
        Local3,

        // World transformations
        // VK_FORMAT_R32G32B32A32_SFLOAT
        World0,
        World1,
        World2,
        World3
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

        static std::span<VertexDescription const, 2> Attributes()
        {
            static const std::array<VertexDescription, 2> attributes
            {
                VertexDescription
                {
                    .attribute = VertexAttribute::Position,
                    .binding = VertexBinding::PerVertex,
                    .offset = offsetof(VertexBasic, position)
                },
                VertexDescription
                {
                    .attribute = VertexAttribute::Color,
                    .binding = VertexBinding::PerVertex,
                    .offset = offsetof(VertexBasic, color)
                }
            };

            return attributes;
        }
    };

    struct VertexInstance
    {
        Matrix4<float> local_transformations;
        Matrix4<float> world_transformations;

        static std::span<VertexDescription const, 8> Attributes()
        {
            static const std::array<VertexDescription, 8> attributes
            {
                VertexDescription
                {
                    .attribute = VertexAttribute::Local0,
                    .binding = VertexBinding::PerInstance,
                    .offset = offsetof(VertexInstance, local_transformations) + (0 * sizeof(Float4))
                },
                VertexDescription
                {
                    .attribute = VertexAttribute::Local1,
                    .binding = VertexBinding::PerInstance,
                    .offset = offsetof(VertexInstance, local_transformations) + (1 * sizeof(Float4))
                },
                VertexDescription
                {
                    .attribute = VertexAttribute::Local2,
                    .binding = VertexBinding::PerInstance,
                    .offset = offsetof(VertexInstance, local_transformations) + (2 * sizeof(Float4))
                },
                VertexDescription
                {
                    .attribute = VertexAttribute::Local3,
                    .binding = VertexBinding::PerInstance,
                    .offset = offsetof(VertexInstance, local_transformations) + (3 * sizeof(Float4))
                },
                VertexDescription
                {
                    .attribute = VertexAttribute::World0,
                    .binding = VertexBinding::PerInstance,
                    .offset = offsetof(VertexInstance, world_transformations) + (0 * sizeof(Float4))
                },
                VertexDescription
                {
                    .attribute = VertexAttribute::World1,
                    .binding = VertexBinding::PerInstance,
                    .offset = offsetof(VertexInstance, world_transformations) + (1 * sizeof(Float4))
                },
                VertexDescription
                {
                    .attribute = VertexAttribute::World2,
                    .binding = VertexBinding::PerInstance,
                    .offset = offsetof(VertexInstance, world_transformations) + (2 * sizeof(Float4))
                },
                VertexDescription
                {
                    .attribute = VertexAttribute::World3,
                    .binding = VertexBinding::PerInstance,
                    .offset = offsetof(VertexInstance, world_transformations) + (3 * sizeof(Float4))
                }
            };

            return attributes;
        }
    };

} // Rc::Gfx