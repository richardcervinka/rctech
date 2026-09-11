#pragma once

#include <span>
#include <cstddef>
#include <array>
#include "core.h"
#include "std26/inplace_vector.h"

namespace Rc
{
    // static_assert(TODO: check endianity);

    struct KtxHeader
    {
        std::array<uint8_t, 12> identifier;
        uint32_t vk_format;
        uint32_t type_size;
        uint32_t pixel_width;
        uint32_t pixel_height;
        uint32_t pixel_depth;
        uint32_t layer_count;
        uint32_t face_count;
        uint32_t level_count;
        uint32_t supercompression_scheme;

        // Index 
        uint32_t dfd_byte_offset;
        uint32_t dfd_byte_length;
        uint32_t kvd_byte_offset;
        uint32_t kvd_byte_length;
        uint64_t sgd_byte_offset;
        uint64_t sgd_byte_length;
    };

    struct KtxLevel
    {
        uint64_t byte_offset;
        uint64_t byte_length;
        uint64_t uncompressed_byte_length;
    };

    class KtxReader
    {
    public:
        KtxReader(std::span<std::byte const> src);

        PixelFormat Format() const;

        // Pixel width
        uint32_t Width() const;

        // Pixel height
        uint32_t Height() const;

        // MIP level
        uint32_t LevelCount() const;

        std26::inplace_vector<TextureLayout, 16> Layout() const;

        std::span<std::byte const> ImageData() const;

        TextureInfo Info() const
        {
            return
            {
                .data = src,
                .format = Format(),
                .width = Width(),
                .height = Height(),
                .layout = Layout()
            };
        }

    private:
        KtxHeader const& Header() const
        {
            return *reinterpret_cast<KtxHeader const*>(src.data());
        }

        std::span<std::byte const> src;
    };

} // Rc