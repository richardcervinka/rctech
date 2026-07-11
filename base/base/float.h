#pragma once

#include <span>
#include <cstddef>

namespace Rc
{
    struct Float4
    {
        float x {};
        float y {};
        float z {};
        float w {};

        void Store(std::byte* dst) const
        {
            std::memcpy(dst, this, 4 * sizeof(float));
        }
    };

    struct Float3
    {
        float x {};
        float y {};
        float z {};

        void Store(std::byte* dst) const
        {
            std::memcpy(dst, this, 3 * sizeof(float));
        }
    };

    struct Float2
    {
        float x {};
        float y {};

        void Store(std::byte* dst) const
        {
            std::memcpy(dst, this, 2 * sizeof(float));
        }
    };

} // Rc