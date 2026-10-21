#pragma once

#include <cstdint>
#include <array>
#include <span>
#include "base/float.h"
#include "base/matrix.h"

namespace Rc::Gfx
{
    enum class VertexAttribute : uint32_t
    {
        // VK_FORMAT_R32G32B32_SFLOAT
        Position = 0,

        // VK_FORMAT_R32G32B32_SFLOAT
        Normal = 1,

        // VK_FORMAT_R32G32B32_SFLOAT
        Color = 2,

        // Local transformations
        // VK_FORMAT_R32G32B32A32_SFLOAT
        Local0 = 3,
        Local1 = 4,
        Local2 = 5,
        Local3 = 6,

        // World transformations
        // VK_FORMAT_R32G32B32A32_SFLOAT
        World0 = 7,
        World1 = 8,
        World2 = 9,
        World3 = 10,

        // Texture Coordinates
        TextureUV = 11,

        // Material (or texture descriptor) index.
        Material = 12
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
        Float2 uv;

        static constexpr uint32_t stride {
            (sizeof(float) * 3) +
            (sizeof(float) * 2)
        };

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
                    .attribute = VertexAttribute::TextureUV,
                    .binding = VertexBinding::PerVertex,
                    .offset = offsetof(VertexBasic, uv)
                }
            };

            return attributes;
        }
    };

    struct VertexInstance
    {
        Matrix4<float> local_transformations;
        Matrix4<float> world_transformations;
        uint32_t material {0};

        static constexpr uint32_t stride {
            sizeof(Matrix4<float>) +
            sizeof(Matrix4<float>) +
            sizeof(uint32_t)
        };

        static std::span<VertexDescription const, 9> Attributes()
        {
            static const std::array<VertexDescription, 9> attributes
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
                },
                VertexDescription
                {
                    .attribute = VertexAttribute::Material,
                    .binding = VertexBinding::PerInstance,
                    .offset = offsetof(VertexInstance, material)
                }
            };

            return attributes;
        }
    };

} // Rc::Gfx