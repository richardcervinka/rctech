#include "resources.h"
#include <array>
#include <cstddef>

namespace Rc::Res::Vs
{
    std::span<uint32_t const> Dummy()
    {
        static constexpr uint8_t data[]
        {
            #embed "${CMAKE_BINARY_DIR}/shaders/vs_dummy.spv"
        };

        static_assert(sizeof(data) % sizeof(uint32_t) == 0);
        return {reinterpret_cast<uint32_t const*>(data), sizeof(data) / sizeof(uint32_t)};
    }

    std::span<uint32_t const> Overlay()
    {
        static constexpr uint8_t data[]
        {
            #embed "${CMAKE_BINARY_DIR}/shaders/vs_overlay.spv"
        };
        
        static_assert(sizeof(data) % sizeof(uint32_t) == 0);
        return {reinterpret_cast<uint32_t const*>(data), sizeof(data) / sizeof(uint32_t)};
    }

    std::span<uint32_t const> Test()
    {
        static constexpr uint8_t data[]
        {
            #embed "${CMAKE_BINARY_DIR}/shaders/vs_test.spv"
        };

        static_assert(sizeof(data) % sizeof(uint32_t) == 0);
        return {reinterpret_cast<uint32_t const*>(data), sizeof(data) / sizeof(uint32_t)};
    }
    
} // namespace Rc::Res::Vs

namespace Rc::Res::Ps
{
    std::span<uint32_t const> Dummy()
    {
        static constexpr uint8_t data[]
        {
            #embed "${CMAKE_BINARY_DIR}/shaders/ps_dummy.spv"
        };

        static_assert(sizeof(data) % sizeof(uint32_t) == 0);
        return {reinterpret_cast<uint32_t const*>(data), sizeof(data) / sizeof(uint32_t)};
    }
    
} // namespace Rc::Res::Ps