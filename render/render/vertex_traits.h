#pragma once

#include "vulkan/device.h"
#include "core/vertex.h"
#include <array>

namespace Rc::Render
{
    template<typename T>
    struct Traits;

    template<>
    struct Traits<VertexBasic>
    {
        inline static const std::array<VkVertexInputAttributeDescription, 2> attributes
        {{
            {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexBasic, position)},
            {1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexBasic, color)}
        }};
    };

} // Rc::Render