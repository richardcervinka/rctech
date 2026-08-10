#include "texture.h"
#include "error.h"

namespace Rc::Render
{
    static VkFormat ToVkFormat(PixelFormat format)
    {
        switch (format)
        {
            case PixelFormat::DepthBuffer:
                return VK_FORMAT_D32_SFLOAT;
        }

        std::unreachable();
    }

    Texture2D::Texture2D(
        VulkanDevice const& vk_device,
        VmaAllocator vma_allocator,
        PixelFormat format,
        uint32_t width,
        uint32_t height,
        VkImageUsageFlags usage, // VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
        VkImageLayout layout  // VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL / VK_IMAGE_LAYOUT_UNDEFINED
    ) :
        vk_device{vk_device},
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
            .mipLevels = 1,
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

    Texture2D::~Texture2D()
    {
        if (vma_allocator != VK_NULL_HANDLE)
        {
            vmaDestroyImage(vma_allocator, vk_image, vma_allocation);
        }
    }

    uint32_t Texture2D::Channels() const
    {
        // TODO: Format to number of channels
    };

    std::unique_ptr<RenderTargetView> Texture2D::CreateDepthBufferView() const
    {
        return std::make_unique<RenderTargetView>(
            vk_device,
            vk_image,
            vk_format,
            VK_IMAGE_ASPECT_DEPTH_BIT,
            width,
            height
        );
    }

} // Rc::Render