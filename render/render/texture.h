#pragma once

#include "vulkan/device.h"
#include "core/core.h"
#include "render_target.h"

namespace Rc::Render
{
    class Texture2d
    {
    public:
        Texture2d(
            VulkanDevice const& vk_device,
            VmaAllocator vma_allocator,
            PixelFormat format,
            uint32_t width,
            uint32_t height,
            uint32_t mip_levels
        );

        ~Texture2d();

        VkImage const& Underlying() const
        {
            return vk_image;
        }

        PixelFormat Format() const
        {
            return format;
        }

        VkFormat UnderlyingFormat() const
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

        uint32_t MipLevels() const
        {
            return mip_levels;
        }

        uint32_t Channels() const;

        // Total number of bytes in linear image layout.
        // Use this value to allocate staging buffer.
        uint64_t GetLinearDataSize() const;

        std::unique_ptr<RenderTargetView> CreateDepthBufferView() const;

    private:
        friend class TransferCommandBuffer;
        friend class RenderTargetView;

        VulkanDevice const& vk_device;
        VmaAllocator vma_allocator {VK_NULL_HANDLE};
        VmaAllocation vma_allocation {nullptr};
        VmaAllocationInfo vma_allocation_info {};

        PixelFormat format {};
        VkFormat vk_format {VK_FORMAT_UNDEFINED};
        uint32_t width {0};
        uint32_t height {0};
        uint32_t mip_levels {1};

        VkImage vk_image {VK_NULL_HANDLE};
    };

} // Rc::Render
