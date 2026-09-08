#pragma once

#include <cstdint>

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
        SurfaceRGBA

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
    
} // Rc