#include "ktx.h"
#include <stdexcept>

namespace Rc
{
    KtxReader::operator bool() const
    {
        static std::array<uint8_t, 12> const identifier
        {
            0xAB, 0x4B, 0x54, 0x58,
            0x20, 0x32, 0x30, 0xBB,
            0x0D, 0x0A, 0x1A, 0x0A
        };

        return Header().identifier == identifier;
    }

    PixelFormat KtxReader::Format() const
    {
        switch (Header().vk_format)
        {
            case 43: // VK_FORMAT_R8G8B8A8_SRGB
                return PixelFormat::ColorSRGBA;
            case 126: // VK_FORMAT_D32_SFLOAT
                return PixelFormat::DepthFloat;
            case 146: // VK_FORMAT_BC7_SRGB_BLOCK
                return PixelFormat::ColorBC7;
        }

        throw std::runtime_error("KTX format not supported");
    }

    uint32_t KtxReader::Width() const
    {
        return Header().pixel_width;
    }

    uint32_t KtxReader::Height() const
    {
        return Header().pixel_height;
    }

    uint32_t KtxReader::LevelCount() const
    {
        return Header().level_count;
    }

    std26::inplace_vector<TextureLayout, 16> KtxReader::Layout() const
    {
        // TODO: Check size

        auto const raw = src.subspan(sizeof(KtxHeader));
        auto const* level = reinterpret_cast<KtxLevel const*>(raw.data());

        uint32_t width = Width();
        uint32_t height = Height();
        uint32_t const level_count = LevelCount();

        std26::inplace_vector<TextureLayout, 16> result;

        for (uint32_t mip_level = 0; mip_level < level_count; mip_level++)
        {
            result.push_back({
                .width = width,
                .height = height,
                .mip_level = mip_level,
                .array_level = 0,
                .offset = static_cast<uint32_t>(level->byte_offset),  // -------------- Cast!!!
                .size = static_cast<uint32_t>(level->uncompressed_byte_length) // -------------- Cast!!!
            });

            width = std::max<uint32_t>(1, width / 2);
            height = std::max<uint32_t>(1, height / 2);
            level += 1;
        }

        return result;
    }

} // Rc