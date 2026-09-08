#include "texture.h"
#include "error.h"
#include "base/math.h"

namespace Rc::Render
{
    static VkFormat ToVkFormat(PixelFormat format)
    {
        switch (format)
        {
            case PixelFormat::DepthFloat:
                return VK_FORMAT_D32_SFLOAT;
            case PixelFormat::ColorSRGBA:
                return VK_FORMAT_R8G8B8A8_SRGB;
            case PixelFormat::SurfaceBGRA:
                return VK_FORMAT_B8G8R8A8_SRGB;
            case PixelFormat::SurfaceRGBA:
                return VK_FORMAT_R8G8B8A8_SRGB;
        }

        std::unreachable();
    }

    static VkImageUsageFlags ToVkImageUsageFlags(PixelFormat format)
    {
        switch (format)
        {
            case PixelFormat::DepthFloat:
                return VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            case PixelFormat::ColorSRGBA:
                return VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
            case PixelFormat::SurfaceBGRA:
                return 0;
            case PixelFormat::SurfaceRGBA:
                return 0;
        }

        std::unreachable();
    }

    static uint32_t TextureSize(PixelFormat format, uint32_t width, uint32_t height)
    {
        switch (format)
        {
            case PixelFormat::DepthFloat:
                return 4 * width * height;
            case PixelFormat::ColorSRGBA:
                return 4 * width * height;
            case PixelFormat::SurfaceBGRA:
                return 4 * width * height;
            case PixelFormat::SurfaceRGBA:
                return 4 * width * height;
        }

        std::unreachable();
    }

    Texture2d::Texture2d(
        VulkanDevice const& vk_device,
        VmaAllocator vma_allocator,
        PixelFormat format,
        uint32_t width,
        uint32_t height,
        bool mips
    ) :
        vk_device{vk_device},
        format{format},
        vk_format{ToVkFormat(format)},
        width{width},
        height{height}
    {
        if (mips)
        {
            mip_levels = static_cast<uint32_t>(Math::Log2(std::max(width, height))) + 1u;
        }

        VmaAllocationCreateInfo alloc_info {};
        alloc_info.usage = VMA_MEMORY_USAGE_AUTO;
        alloc_info.flags = 0;

        VkImageCreateInfo create_info {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .imageType = VK_IMAGE_TYPE_2D,
            .format = vk_format,
            .extent = {width, height, 1},
            .mipLevels = mip_levels,
            .arrayLayers = 1,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = ToVkImageUsageFlags(format),
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = nullptr,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
        };

        auto const vk_result = vmaCreateImage(
            vma_allocator,
            &create_info,
            &alloc_info,
            &vk_image,
            &vma_allocation,
            &vma_allocation_info
        );

        if (vk_result != VK_SUCCESS)
        {
            throw VulkanException(vk_result);
        }

        this->vma_allocator = vma_allocator;   
    }

    Texture2d::~Texture2d()
    {
        if (vma_allocator != VK_NULL_HANDLE)
        {
            vmaDestroyImage(vma_allocator, vk_image, vma_allocation);
        }
    }

    uint32_t Texture2d::Channels() const
    {
        // TODO: Format to number of channels
    };

    std::unique_ptr<RenderTargetView> Texture2d::CreateDepthBufferView() const
    {
        return std::make_unique<RenderTargetView>(
            vk_device,
            vk_image,
            ToVkFormat(format),
            VK_IMAGE_ASPECT_DEPTH_BIT,
            width,
            height
        );
    }

    uint32_t Texture2d::MipSize(uint32_t mip_level) const
    {
        return TextureSize(format, MipWidth(mip_level), MipHeight(mip_level));
    }

} // Rc::Render