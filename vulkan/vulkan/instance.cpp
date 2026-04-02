#include "instance.h"
#include "context.h"
#include "device.h"
#include <cassert>

namespace Rc
{
    VulkanInstance::VulkanInstance(VulkanContext const& context, VkInstance vk_instance) :
        m_context{context},
        m_vk_instance{vk_instance}
    {
        assert(vk_instance != VK_NULL_HANDLE);

        Load("vkDestroyInstance", m_vkDestroyInstance);
        Load("vkGetPhysicalDeviceProperties", m_vkGetPhysicalDeviceProperties);
        Load("vkEnumeratePhysicalDevices", m_vkEnumeratePhysicalDevices);
        Load("vkCreateWin32SurfaceKHR", m_vkCreateWin32SurfaceKHR);
        Load("vkDestroySurfaceKHR", m_vkDestroySurfaceKHR);
        Load("vkEnumerateDeviceExtensionProperties", m_vkEnumerateDeviceExtensionProperties);
        Load("vkGetPhysicalDeviceQueueFamilyProperties", m_vkGetPhysicalDeviceQueueFamilyProperties);
        Load("vkGetPhysicalDeviceWin32PresentationSupportKHR", m_vkGetPhysicalDeviceWin32PresentationSupportKHR);
        Load("vkGetPhysicalDeviceSurfaceSupportKHR", m_vkGetPhysicalDeviceSurfaceSupportKHR);
        Load("vkCreateDevice", m_vkCreateDevice);
        Load("vkCreateDebugUtilsMessengerEXT", m_vkCreateDebugUtilsMessengerEXT);
        Load("vkDestroyDebugUtilsMessengerEXT", m_vkDestroyDebugUtilsMessengerEXT);
    }

    VulkanInstance::~VulkanInstance()
    {
        m_vkDestroyInstance(m_vk_instance, nullptr);
    }

    uint32_t VulkanInstance::EnumeratePhysicalDevicesCount() const
    {
        uint32_t count {};
        if (auto vk_result = m_vkEnumeratePhysicalDevices(m_vk_instance, &count, nullptr); vk_result != VK_SUCCESS)
        {
            throw VulkanError(vk_result);
        }
        return count;
    }

    std::span<VkPhysicalDevice> VulkanInstance::EnumeratePhysicalDevices(std::span<VkPhysicalDevice> buffer) const
    {
        auto count = static_cast<uint32_t>(buffer.size());
        if (auto vk_result = m_vkEnumeratePhysicalDevices(m_vk_instance, &count, buffer.data()); vk_result != VK_SUCCESS)
        {
            throw VulkanError(vk_result);
        }
        return buffer.subspan(0, count);
    }

    VkSurfaceKHR VulkanInstance::CreateWin32SurfaceKHR(VkWin32SurfaceCreateInfoKHR const& create_info) const
    {
        VkSurfaceKHR result = VK_NULL_HANDLE;
        if (auto vk_result = m_vkCreateWin32SurfaceKHR(m_vk_instance, &create_info, nullptr, &result); vk_result != VK_SUCCESS)
        {
            throw VulkanError(vk_result);
        }
        return result;
    }

    void VulkanInstance::DestroySurfaceKHR(VkSurfaceKHR& surface) const noexcept
    {
        m_vkDestroySurfaceKHR(m_vk_instance, surface, nullptr);
        surface = VK_NULL_HANDLE;
    }

    VkPhysicalDeviceProperties VulkanInstance::GetPhysicalDeviceProperties(VkPhysicalDevice physical_device) const
    {
        VkPhysicalDeviceProperties result {};
        m_vkGetPhysicalDeviceProperties(physical_device, &result);
        return result;
    }

    VkPhysicalDeviceMemoryProperties VulkanInstance::GetPhysicalDeviceMemoryProperties(VkPhysicalDevice physical_device) const
    {
        VkPhysicalDeviceMemoryProperties result {};
        m_vkGetPhysicalDeviceMemoryProperties(physical_device, &result);
        return result;
    }

    uint32_t VulkanInstance::EnumerateDeviceExtensionPropertiesCount(VkPhysicalDevice physical_device, std::string_view layer_name) const
    {
        uint32_t result {};
        if (auto vk_result = m_vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &result, nullptr); vk_result != VK_SUCCESS)
        {
            throw VulkanError(vk_result);
        }
        return result;
    }

    std::span<VkExtensionProperties> VulkanInstance::EnumerateDeviceExtensionProperties(VkPhysicalDevice physical_device, std::string_view layer_name, std::span<VkExtensionProperties> buffer) const
    {
        auto count = static_cast<uint32_t>(buffer.size());
        if (auto vk_result = m_vkEnumerateDeviceExtensionProperties(physical_device, layer_name.data(), &count, buffer.data()); vk_result != VK_SUCCESS)
        {
            throw VulkanError(vk_result);
        }
        return buffer.subspan(0, count);
    }

    uint32_t VulkanInstance::GetPhysicalDeviceQueueFamilyPropertiesCount(VkPhysicalDevice physical_device) const
    {
        uint32_t result {};
        m_vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &result, nullptr);
        return result;
    }

    std::span<VkQueueFamilyProperties> VulkanInstance::GetPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice physical_device, std::span<VkQueueFamilyProperties> buffer) const
    {
        auto count = static_cast<uint32_t>(buffer.size());
        m_vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &count, buffer.data());
        return buffer.subspan(0, count);
    }

    bool VulkanInstance::GetPhysicalDevicePresentationSupport(VkPhysicalDevice physical_device, uint32_t queueFamilyIndex) const
    {
    #ifdef VK_USE_PLATFORM_WIN32_KHR
        return m_vkGetPhysicalDeviceWin32PresentationSupportKHR(physical_device, queueFamilyIndex) == VK_TRUE;
    #else
        return true;
    #endif
    }

    bool VulkanInstance::GetPhysicalDeviceSurfaceSupportKHR(VkPhysicalDevice physical_device, uint32_t queue_family_index, VkSurfaceKHR surface) const
    {
        VkBool32 result {};
        if (auto vk_result = m_vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, queue_family_index, surface, &result); vk_result != VK_SUCCESS)
        {
            throw VulkanError(vk_result);
        }
        return result == VK_TRUE;
    }

    std::unique_ptr<VulkanDevice> VulkanInstance::CreateDevice(VkPhysicalDevice physical_device, VkDeviceCreateInfo const& create_info) const
    {
        VkDevice result = VK_NULL_HANDLE;
        if (auto vk_result = m_vkCreateDevice(physical_device, &create_info, nullptr, &result); vk_result != VK_SUCCESS)
        {
            throw VulkanError(vk_result);
        }
        return std::make_unique<VulkanDevice>(m_context, result);
    }

    VkDebugUtilsMessengerEXT VulkanInstance::CreateDebugUtilsMessengerEXT(VkDebugUtilsMessengerCreateInfoEXT const& create_info) const
    {
        VkDebugUtilsMessengerEXT result = VK_NULL_HANDLE;
        if (auto vk_result = m_vkCreateDebugUtilsMessengerEXT(m_vk_instance, &create_info, nullptr, &result); vk_result != VK_SUCCESS)
        {
            throw VulkanError(vk_result);
        }
        return result;
    }

    void VulkanInstance::DestroyDebugUtilsMessengerEXT(VkDebugUtilsMessengerEXT& messenger) const
    {
        m_vkDestroyDebugUtilsMessengerEXT(m_vk_instance, messenger, nullptr);
        messenger = VK_NULL_HANDLE;
    }

} // Rc