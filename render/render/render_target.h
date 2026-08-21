#pragma once

#include "vulkan/device.h"

namespace Rc::Render
{
    class RenderTargetView
    {
    public:
        RenderTargetView() = default;
        ~RenderTargetView();

        RenderTargetView(
            VulkanDevice const& vk_device,
            VkImage vk_image,
            VkFormat vk_format,
            VkImageAspectFlags aspect,
            int width,
            int height
        );

        RenderTargetView(RenderTargetView const&) = delete;
        RenderTargetView& operator=(RenderTargetView const&) = delete;
        RenderTargetView(RenderTargetView&& other) = delete;
        RenderTargetView& operator=(RenderTargetView&& other) = delete;

        VkImageView View() const
        {
            return view;
        }

        VkImage Underlying() const
        {
            return image;
        }

        VkImageLayout Layout() const
        {
            return layout;
        }

        int Width() const
        {
            return width;
        }

        int Height() const
        {
            return height;
        }

    private:
        friend class RenderCommandBuffer;

        VulkanDevice const& vk_device;

        // Non-owned vk image.
        VkImage image {VK_NULL_HANDLE};
        
        VkImageView view {VK_NULL_HANDLE};

        mutable VkImageLayout layout {VK_IMAGE_LAYOUT_UNDEFINED}; //---------------------------
        VkImageAspectFlags vk_aspect {VK_IMAGE_ASPECT_NONE};

        int width {0};
        int height {0};
    };

} // Rc::Render
