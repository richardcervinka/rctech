#pragma once

#include <array>
#include <cstdint>
#include <span>

// Embedded resources storage.

namespace Res::Vs
{
    std::span<uint32_t const> Dummy();
    std::span<uint32_t const> Overlay();
    std::span<uint32_t const> Test();

} // namespace Res::Vs

namespace Res::Ps
{
    std::span<uint32_t const> Dummy();

} // namespace Res::Ps