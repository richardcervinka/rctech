#include "texture.h"
#include "error.h"

namespace Rc::Render
{
    RenderTargetView::RenderTargetView(VulkanDevice const& vk_device, VkImage vk_image, VkFormat vk_format, int width, int height) :
        m_vk_device{&vk_device},
        m_width{width},
        m_height{height}
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
        m_image = vk_image;
    }

    RenderTargetView::~RenderTargetView()
    {
        if (m_vk_device)
        {
            m_vk_device->DestroyImageView(m_view);
        }
    }

    // RenderTargetView::RenderTargetView(RenderTargetView&& other) noexcept :
    //     m_vk_device{other.m_vk_device},
    //     m_view{other.m_view}
    // {
    //     other.m_vk_device = nullptr;
    //     other.m_view = VK_NULL_HANDLE;
    // }

    // RenderTargetView& RenderTargetView::operator=(RenderTargetView&& other) noexcept
    // {
    //     std::swap(m_vk_device, other.m_vk_device);
    //     std::swap(m_view, other.m_view);
    //     return *this;
    // }

    std::unique_ptr<RenderTargetView> Texture2D::CreateView() const
    {
        return std::make_unique<RenderTargetView>(*m_vk_device, m_vk_image, m_vk_format, m_width, m_height);
    }

} // Rc::Render