#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include "std26/inplace_vector.h"

namespace Rc
{    
    enum class PixelFormat
    {
        // D32
        DepthFloat,

        // RGBA8_SRGB texture
        ColorSRGBA,

        // SwapChain surface BGRA8_SRGB
        SurfaceBGRA,

        // SwapChain surface RGBA_SRGB
        SurfaceRGBA,

        // BC7 SRGB
        ColorBC7

        // RGB8 UNORM texture
        // LinearRGB

        // RGBA8 UNORM texture
        // LinearRGBA

        // HdrRGB
        // HdrRGBA
    };

    struct TextureLayout
    {
        uint32_t width {0};
        uint32_t height {0};
        uint32_t mip_level {0};
        uint32_t array_level {0};
        uint32_t offset {0};
        uint32_t size {0};
    };

    struct TextureInfo
    {
        static constexpr uint32_t max_mip_levels = 16;

        std::span<std::byte const> data;
        PixelFormat format {PixelFormat::ColorSRGBA};
        uint32_t width {0};
        uint32_t height {0};
        std26::inplace_vector<TextureLayout, max_mip_levels> layout;
    };
    
} // Rc