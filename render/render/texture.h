#pragma once

#include "vulkan/device.h"

namespace Rc::Render
{    
    class Texture2D;


    class RenderTargetView
    {
    public:
        RenderTargetView() = default;
        ~RenderTargetView();

        RenderTargetView(VulkanDevice const& vk_device, VkImage vk_image, VkFormat vk_format, int width, int height);

        RenderTargetView(RenderTargetView const&) = delete;
        RenderTargetView& operator=(RenderTargetView const&) = delete;
        RenderTargetView(RenderTargetView&& other) = delete;
        RenderTargetView& operator=(RenderTargetView&& other) = delete;

        VkImageView View() const
        {
            return view;
        }

        VkImage Image() const
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

        VulkanDevice const* vk_device {nullptr};

        // Non-owned vk image.
        VkImage image {VK_NULL_HANDLE};
        
        VkImageView view {VK_NULL_HANDLE};

        mutable VkImageLayout layout {VK_IMAGE_LAYOUT_UNDEFINED};

        int width {0};
        int height {0};
    };


    class Texture2D
    {
    public:
        Texture2D() = default;

        Texture2D(
            VulkanDevice const& vk_device,
            VkImage vk_image,
            VkFormat vk_format,
            int width,
            int height)
            :   vk_device{&vk_device},
                vk_image{vk_image},
                vk_format{vk_format},
                width{width},
                height{height}
        {}

        VkImage const& GetImage() const
        {
            return vk_image;
        }

        VkFormat const& GetFormat() const
        {
            return vk_format;
        }

        std::unique_ptr<RenderTargetView> CreateView() const;

    private:
        friend class RenderTargetView;

        VulkanDevice const* vk_device {nullptr};

        // Non-owning image handle.
        VkImage vk_image {VK_NULL_HANDLE};
        
        VkFormat vk_format {VK_FORMAT_UNDEFINED};

        int width {0};
        int height {0};
    };

} // Rc::Render
