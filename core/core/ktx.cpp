#include "ktx.h"

namespace Rc
{
    KtxReader::operator bool() const
    {
        static std::array<uint8_t, 12> const identifier
        {
            0xAB, 0x4B, 0x54, 0x58, 0x20, 0x32, 0x30, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A
        };

        return Header().identifier == identifier;
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
        std26::inplace_vector<TextureLayout, 16> result;

        

        return result;
    }

} // Rc