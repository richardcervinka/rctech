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

} // namespace Rc::Res::Vs

namespace Rc::Res::Ps
{
    std::span<uint32_t const> Dummy();

} // namespace Rc::Res::Ps

namespace Rc::Res::Textures
{
    TextureInfo Default256x256sRgba();

} // namespace Rc::Res::Ps