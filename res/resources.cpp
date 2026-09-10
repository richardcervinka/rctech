#include "resources.h"
#include "core/ktx.h"

namespace Rc::Res::Vs
{
    // std::span<uint32_t const> Overlay()
    // {
    //     alignas(uint32_t) static constexpr uint8_t data[]
    //     {
    //         #embed "${CMAKE_BINARY_DIR}/shaders/vs_overlay.spv"
    //     };
        
    //     static_assert(sizeof(data) % sizeof(uint32_t) == 0);
    //     return {reinterpret_cast<uint32_t const*>(data), sizeof(data) / sizeof(uint32_t)};
    // }

    std::span<uint32_t const> Test()
    {
        alignas(uint32_t) static constexpr uint8_t data[]
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
        alignas(uint32_t) static constexpr uint8_t data[]
        {
            #embed "${CMAKE_BINARY_DIR}/shaders/ps_dummy.spv"
        };

        static_assert(sizeof(data) % sizeof(uint32_t) == 0);
        return {reinterpret_cast<uint32_t const*>(data), sizeof(data) / sizeof(uint32_t)};
    }
    
} // namespace Rc::Res::Ps

namespace Rc::Res::Textures
{
    //static consteval GetTextureInfo()
    TextureInfo Default256x256sRgba()
    {
        static const uint8_t data[]
        {
            #embed "${CMAKE_SOURCE_DIR}/res/textures/default_256x256.ktx2"
        };

        return KtxReader(std::as_bytes(std::span{data})).Info();
    }

} // namespace Rc::Res::Textures