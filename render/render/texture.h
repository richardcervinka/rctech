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
            return m_view;
        }

        VkImage Image() const
        {
            return m_image;
        }

        VkImageLayout Layout() const
        {
            return m_layout;
        }

        int Width() const
        {
            return m_width;
        }

        int Height() const
        {
            return m_height;
        }

    private:
        friend class RenderCommandBuffer;

        VulkanDevice const* m_vk_device {nullptr};

        // Non-owned vk image.
        VkImage m_image {VK_NULL_HANDLE};
        
        VkImageView m_view {VK_NULL_HANDLE};

        mutable VkImageLayout m_layout {VK_IMAGE_LAYOUT_UNDEFINED};

        int m_width {0};
        int m_height {0};
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
            :   m_vk_device{&vk_device},
                m_vk_image{vk_image},
                m_vk_format{vk_format},
                m_width{width},
                m_height{height}
        {}

        VkImage const& GetImage() const { return m_vk_image; }

        VkFormat const& GetFormat() const { return m_vk_format; }

        std::unique_ptr<RenderTargetView> CreateView() const;

    private:
        friend class RenderTargetView;

        VulkanDevice const* m_vk_device {nullptr};

        // Non-owning image handle.
        VkImage m_vk_image {VK_NULL_HANDLE};
        
        VkFormat m_vk_format {VK_FORMAT_UNDEFINED};

        int m_width {0};
        int m_height {0};
    };

} // Rc::Render
