#pragma once

#include <cstdint>
#include <mdspan>
#include <vector>
#include <filesystem>
#include "color.h"

namespace Rc
{
    class Image
    {
    public:
        Image() = default;
        Image(uint32_t width, uint32_t height);
        Image(uint32_t width, uint32_t height, std::vector<uint8_t> data);

        static Image Load(std::filesystem::path const& path);
        static Image LoadPng(std::filesystem::path const& path);

        void Save(std::filesystem::path const& path);
        void SavePng(std::filesystem::path const& path);

        std::span<uint8_t const> Raw() const
        {
            return data;
        }

        std::span<uint8_t> Raw()
        {
            return data;
        }

        std::mdspan<Rgba, std::dextents<std::size_t, 2>, std::layout_right> Data()
        {
            return std::mdspan{reinterpret_cast<Rgba*>(data.data()), height, width};
        }

        std::mdspan<Rgba const, std::dextents<std::size_t, 2>, std::layout_right> Data() const
        {
            return std::mdspan{reinterpret_cast<Rgba const*>(data.data()), height, width};
        }

        uint32_t Width() const
        {
            return width;
        }

        uint32_t Height() const
        {
            return height;
        }

        Image GenerateMip() const;

        std::vector<Image> GenerateMips() const;

    private:
        uint32_t width {0};
        uint32_t height {0};
        std::vector<uint8_t> data;
    };

} // Rc