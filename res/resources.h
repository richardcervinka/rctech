#pragma once

#include <cstdint>
#include <span>
#include "core/core.h"

// Embedded resources storage.

namespace Rc::Res::Vs
{
    // std::span<uint32_t const> Dummy();
    // std::span<uint32_t const> Overlay();
    std::span<uint32_t const> Test();

} // Rc::Res::Vs

namespace Rc::Res::Ps
{
    std::span<uint32_t const> Dummy();

} // Rc::Res::Ps

namespace Rc::Res::Textures
{
    struct EmbeddedTexture 
    {
        std::span<std::byte const> data;
        TextureInfo info;
    };

    EmbeddedTexture Default256x256sRgba();

} // Rc::Res::Ps