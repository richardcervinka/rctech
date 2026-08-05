#pragma once

#include "vulkan/device.h"

#include "render_target.h"

namespace Rc::Render
{    
    class Texture2D;


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
            :   vk_device{vk_device},
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

        VulkanDevice const& vk_device;

        // Non-owning image handle.
        VkImage vk_image {VK_NULL_HANDLE};
        
        VkFormat vk_format {VK_FORMAT_UNDEFINED};

        int width {0};
        int height {0};
    };

} // Rc::Render
