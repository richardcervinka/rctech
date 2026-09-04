#include "image.h"
#include <cassert>
#include "lodepng.h"

namespace Rc
{
    Image::Image(uint32_t width, uint32_t height) :
        width{width},
        height{height},
        data(width * height * 4)
    {
        assert(width > 0);
        assert(height > 0);
    }

    Image::Image(uint32_t width, uint32_t height, std::vector<uint8_t> data) :
        width{width},
        height{height},
        data{std::move(data)}
    {
        assert(width > 0);
        assert(height > 0);
        assert(this->data.size() == width * height * 4);
    }

    Image Image::Load(std::filesystem::path const& path)
    {
        auto const ext = path.extension();

        if (ext == ".png")
        {
            return LoadPng(path);
        }

        throw std::runtime_error("Invalid image file format");
    }

    Image Image::LoadPng(std::filesystem::path const& path)
    {
        std::vector<unsigned char> png;
        std::vector<unsigned char> image;
        unsigned width {};
        unsigned height {};
        lodepng::State state; //optionally customize this one

        if (auto error = lodepng::load_file(png, path.string()))
        {
            throw std::runtime_error(lodepng_error_text(error));
        }
        if (auto error = lodepng::decode(image, width, height, state, png))
        {
            throw std::runtime_error(lodepng_error_text(error));
        }

        return {width, height, std::move(image)};
    }

    void Image::Save(std::filesystem::path const& path)
    {
        auto const ext = path.extension();

        if (ext == ".png")
        {
            SavePng(path);
        }

        throw std::runtime_error("Invalid image file format");
    }

    void Image::SavePng(std::filesystem::path const& path)
    {
        if (auto error = lodepng::encode(path.string(), data.data(), width, height))
        {
            throw std::runtime_error(lodepng_error_text(error));
        }
    }

    Image Image::GenerateMip() const
    {
        auto data = Data();

        Image result;

        result.width = width / 2;
        result.height = height / 2;
        result.data.reserve(result.width * result.height * 4);

        for (std::size_t row = 0; row < height; row += 2)
        {
            for (std::size_t col = 0; col < width; col += 2)
            {
                auto const a = Color(data[row + 0, col + 0]).DecodeSrgba();
                auto const b = Color(data[row + 0, col + 1]).DecodeSrgba();
                auto const c = Color(data[row + 1, col + 0]).DecodeSrgba();
                auto const d = Color(data[row + 1, col + 1]).DecodeSrgba();

                auto const average = (a + b + c + d) / 4.0;
                Rgba const srgba = average.EncodeSrgba();

                result.data.push_back(srgba.r);
                result.data.push_back(srgba.g);
                result.data.push_back(srgba.b);
                result.data.push_back(srgba.a);
            }
        }

        return result;
    }

    std::vector<Image> Image::GenerateMips() const
    {
        if (Width() == 1 || Height() == 1)
        {
            return {};
        }

        std::vector<Image> mips;
        mips.emplace_back(GenerateMip());

        while ((mips.back().Width() > 1) && (mips.back().Height() > 1))
        {
            mips.push_back(mips.end()->GenerateMip());
        }

        return mips;
    }

} // Rc