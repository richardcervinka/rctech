#include "surface.h"

namespace Rc::Render
{
    Surface::Surface(VulkanInstance const& vk_instance, Window const& window) : vk_instance{vk_instance}
    {
        VkWin32SurfaceCreateInfoKHR const create_info
        {
            .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
            .pNext = nullptr,
            .flags = 0,
            .hinstance = GetModuleHandle(NULL),
            .hwnd = window.Hwnd()
        };

        vk_surface = vk_instance.CreateWin32SurfaceKHR(create_info);
    }

    Surface::~Surface()
    {
        vk_instance.DestroySurfaceKHR(vk_surface);
    }

} // Rc::Render