#include "texture.h"
#include "error.h"

namespace Rc::Render
{
    static VkFormat ToVkFormat(PixelFormat format)
    {
        switch (format)
        {
            case PixelFormat::Depth:
                return VK_FORMAT_D32_SFLOAT;
            case PixelFormat::ColorRGBA:
                return VK_FORMAT_R8G8B8A8_SRGB;
            case PixelFormat::SurfaceBGRA:
                return VK_FORMAT_B8G8R8A8_SRGB;
            case PixelFormat::SurfaceRGBA:
                return VK_FORMAT_R8G8B8A8_SRGB;
        }

        std::unreachable();
    }

    Texture2d::Texture2d(
        VulkanDevice const& vk_device,
        VkImage vk_image,
        PixelFormat format,
        uint32_t width,
        uint32_t height
    ) :
        vk_device{vk_device},
        format{format},
        width{width},
        height{height},
        vk_image{vk_image}
    {}

    Texture2d::Texture2d(
        VulkanDevice const& vk_device,
        VmaAllocator vma_allocator,
        PixelFormat format,
        uint32_t width,
        uint32_t height,
        VkImageUsageFlags usage,
        VkImageLayout layout
    ) :
        vk_device{vk_device},
        format{format},
        vk_format{ToVkFormat(format)},
        width{width},
        height{height}
    {
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
            .mipLevels = 1, // ----------------------------------------------
            .arrayLayers = 1,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = usage,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = nullptr,
            .initialLayout = layout
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

    uint64_t Texture2d::GetLinearDataSize() const
    {
        switch(format)
        {
            case PixelFormat::Depth:
                return uint64_t{width} * uint64_t{height} * uint64_t{4};
            case PixelFormat::ColorRGBA:
            case PixelFormat::SurfaceBGRA:
            case PixelFormat::SurfaceRGBA:
                return uint64_t{width} * uint64_t{height} * uint64_t{4};
        }

        std::unreachable();
    }

} // Rc::Render