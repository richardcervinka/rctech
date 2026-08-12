#pragma once

#include "vulkan/device.h"

#include "render_target.h"

namespace Rc::Render
{    
    enum class PixelFormat
    {
        Depth,

        // RGB8 SRGB texture
        ColorRGB,

        // RGBA8 SRGB texture
        ColorRGBA

        // RGB8 UNORM texture
        // LinearRGB

        // RGBA8 UNORM texture
        // LinearRGBA

        // HdrRGB
        // HdrRGBA
    };

    class Texture2D
    {
    public:
        Texture2D(
            VulkanDevice const& vk_device,
            VmaAllocator vma_allocator,
            PixelFormat format,
            uint32_t width,
            uint32_t height,
            VkImageUsageFlags usage,
            VkImageLayout layout
        );

        ~Texture2D();

        VkImage const& GetImage() const
        {
            return vk_image;
        }

        VkFormat Format() const
        {
            return vk_format;
        }

        uint32_t Width() const
        {
            return width;
        }

        uint32_t Height() const
        {
            return height;
        }

        uint32_t Channels() const;

        std::unique_ptr<RenderTargetView> CreateDepthBufferView() const;

    private:
        friend class RenderTargetView;

        VulkanDevice const& vk_device;
        VmaAllocator vma_allocator {VK_NULL_HANDLE};
        VmaAllocation vma_allocation {nullptr};
        VmaAllocationInfo vma_allocation_info {};

        VkFormat vk_format {VK_FORMAT_UNDEFINED};
        uint32_t width {0};
        uint32_t height {0};

        VkImage vk_image {VK_NULL_HANDLE};

        
    };

} // Rc::Render
