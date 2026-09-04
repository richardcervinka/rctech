#pragma once

namespace Rc
{    
    enum class PixelFormat
    {
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
    
} // Rc