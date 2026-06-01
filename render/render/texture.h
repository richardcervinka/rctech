#pragma once

#include "vulkan/device.h"

namespace Rc::Render
{    
    class Texture2D;

    class TextureView2D
    {
    public:
        TextureView2D() = default;
        ~TextureView2D();

        TextureView2D(VulkanDevice const& vk_device, VkImage vk_image, VkFormat vk_format);

        TextureView2D(TextureView2D const&) = delete;
        TextureView2D& operator=(TextureView2D const&) = delete;
        TextureView2D(TextureView2D&& other) = delete;
        TextureView2D& operator=(TextureView2D&& other) = delete;

        VkImageView GetView() const { return m_view; }

    private:
        VulkanDevice const* m_vk_device {nullptr};
        //Texture2D* m_texture {nullptr};

        VkImageView m_view {VK_NULL_HANDLE};
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

        std::unique_ptr<TextureView2D> CreateView() const;

    private:
        friend class TextureView2D;

        VulkanDevice const* m_vk_device {nullptr};

        // Non-owning image handle.
        VkImage m_vk_image {VK_NULL_HANDLE};
        
        VkFormat m_vk_format {VK_FORMAT_UNDEFINED};

        int m_width {0};
        int m_height {0};
    };

} // Rc::Render
