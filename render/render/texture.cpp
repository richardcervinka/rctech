#include "texture.h"
#include "error.h"

namespace Rc::Render
{
    TextureView2D::TextureView2D(VulkanDevice const& vk_device, VkImage vk_image, VkFormat vk_format) :
        m_vk_device{&vk_device}
    {
        VkImageViewCreateInfo const create_info
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .image = vk_image,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = vk_format,
            .components = {
                VK_COMPONENT_SWIZZLE_IDENTITY,
                VK_COMPONENT_SWIZZLE_IDENTITY,
                VK_COMPONENT_SWIZZLE_IDENTITY,
                VK_COMPONENT_SWIZZLE_IDENTITY
            },
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        };

        m_view = m_vk_device->CreateImageView(create_info);
    }

    TextureView2D::~TextureView2D()
    {
        if (m_vk_device)
        {
            m_vk_device->DestroyImageView(m_view);
        }
    }

    // TextureView2D::TextureView2D(TextureView2D&& other) noexcept :
    //     m_vk_device{other.m_vk_device},
    //     m_view{other.m_view}
    // {
    //     other.m_vk_device = nullptr;
    //     other.m_view = VK_NULL_HANDLE;
    // }

    // TextureView2D& TextureView2D::operator=(TextureView2D&& other) noexcept
    // {
    //     std::swap(m_vk_device, other.m_vk_device);
    //     std::swap(m_view, other.m_view);
    //     return *this;
    // }

    std::unique_ptr<TextureView2D> Texture2D::CreateView() const
    {
        return std::make_unique<TextureView2D>(*m_vk_device, m_vk_image, m_vk_format);
    }

} // Rc::Render