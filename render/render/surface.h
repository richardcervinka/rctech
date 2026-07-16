#pragma once

#include "vulkan/instance.h"
#include "platform/window.h"

namespace Rc::Render
{
    class Surface
    {
    public:
        Surface(VulkanInstance const& vk_instance, Window const& window);

        ~Surface();

        Surface(Surface const&) = delete;
        Surface& operator=(Surface const&) = delete;
        Surface(Surface&& other) = delete;
        Surface& operator=(Surface&& other) = delete;

        VkSurfaceKHR Handle() const
        {
            return vk_surface;
        }

    private:
        friend class Device;

        VulkanInstance const& vk_instance;
        VkSurfaceKHR vk_surface {VK_NULL_HANDLE};
    };

} // Rc::Render