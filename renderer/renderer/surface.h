#pragma once

#include "vulkan/instance.h"
#include "platform/window.h"

namespace Rc
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

    private:
        friend class Device; // ------- review

        VulkanInstance const* m_vk_instance {nullptr};
        VkSurfaceKHR m_vk_surface {VK_NULL_HANDLE};
    };

} // Rc