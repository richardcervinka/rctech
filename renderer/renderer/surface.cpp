#include "surface.h"

namespace Rc
{
    Surface::Surface(VulkanInstance const& vk_instance, Window const& window) :
        m_vk_instance{&vk_instance}
    {
        VkWin32SurfaceCreateInfoKHR const create_info
        {
            .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
            .pNext = nullptr,
            .flags = 0,
            .hinstance = GetModuleHandle(NULL),
            .hwnd = window.Hwnd()
        };

        m_vk_surface = m_vk_instance->CreateWin32SurfaceKHR(create_info);
    }

    Surface::~Surface()
    {
        if (m_vk_instance != nullptr)
        {
            m_vk_instance->DestroySurfaceKHR(m_vk_surface);
        }
    }

} // Rc