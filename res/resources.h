#pragma once

#include <cstdint>
#include <span>

// Embedded resources storage.

namespace Rc::Res::Vs
{
    std::span<uint32_t const> Dummy();
    std::span<uint32_t const> Overlay();
    std::span<uint32_t const> Test();

} // namespace Rc::Res::Vs

namespace Rc::Res::Ps
{
    std::span<uint32_t const> Dummy();

} // namespace Rc::Res::Ps