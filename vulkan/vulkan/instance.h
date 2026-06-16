#pragma once

#include "context.h"

namespace Rc
{
    class VulkanContext;
    class VulkanDevice;

    class VulkanInstance
    {
    public:
        VulkanInstance(VulkanContext const& context, VkInstance vk_instance);
        ~VulkanInstance();

        VulkanInstance(VulkanInstance const&) = delete;
        VulkanInstance(VulkanInstance&&) = delete;
        VulkanInstance& operator=(VulkanInstance const&) = delete;
        VulkanInstance& operator=(VulkanInstance&&) = delete;

        VkInstance Handle() const { return m_vk_instance; }

        // vkEnumeratePhysicalDevices
        uint32_t EnumeratePhysicalDevicesCount() const;

        // vkEnumeratePhysicalDevices
        std::span<VkPhysicalDevice> EnumeratePhysicalDevices(std::span<VkPhysicalDevice> buffer) const;

        // vkCreateWin32SurfaceKHR
        VkSurfaceKHR CreateWin32SurfaceKHR(VkWin32SurfaceCreateInfoKHR const& create_info) const;

        // vkDestroySurfaceKHR
        void DestroySurfaceKHR(VkSurfaceKHR& surface) const noexcept;

        // vkGetPhysicalDeviceProperties
        VkPhysicalDeviceProperties GetPhysicalDeviceProperties(VkPhysicalDevice physical_device) const;

        // vkGetPhysicalDeviceMemoryProperties
        VkPhysicalDeviceMemoryProperties GetPhysicalDeviceMemoryProperties(VkPhysicalDevice physical_device) const;

        // vkEnumerateDeviceExtensionProperties
        uint32_t EnumerateDeviceExtensionPropertiesCount(VkPhysicalDevice physical_device, std::string const& layer_name) const;

        // vkEnumerateDeviceExtensionProperties
        std::span<VkExtensionProperties> EnumerateDeviceExtensionProperties(VkPhysicalDevice physical_device, std::string const& layer_name, std::span<VkExtensionProperties> buffer) const;

        // vkGetPhysicalDeviceQueueFamilyProperties
        uint32_t GetPhysicalDeviceQueueFamilyPropertiesCount(VkPhysicalDevice physical_device) const;

        // vkGetPhysicalDeviceQueueFamilyProperties
        std::span<VkQueueFamilyProperties> GetPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice physical_device, std::span<VkQueueFamilyProperties> buffer) const;

        // vkGetPhysicalDeviceWin32PresentationSupportKHR
        bool GetPhysicalDevicePresentationSupport(VkPhysicalDevice physical_device, uint32_t queue_family_index) const;

        // vkGetPhysicalDeviceSurfaceSupportKHR
        bool GetPhysicalDeviceSurfaceSupportKHR(VkPhysicalDevice physical_device, uint32_t queue_family_index, VkSurfaceKHR surface) const;

        // vkGetPhysicalDeviceDescriptorSizeEXT
        VkDeviceSize GetPhysicalDeviceDescriptorSizeEXT(VkPhysicalDevice physical_device, VkDescriptorType descriptor_type) const;

        // vkCreateDevice
        std::unique_ptr<VulkanDevice> CreateDevice(VkPhysicalDevice physical_device, VkDeviceCreateInfo const& create_info) const;

        // vkCreateDebugUtilsMessengerEXT
        VkDebugUtilsMessengerEXT CreateDebugUtilsMessengerEXT(VkDebugUtilsMessengerCreateInfoEXT const& create_info) const;

        // vkDestroyDebugUtilsMessengerEXT
        void DestroyDebugUtilsMessengerEXT(VkDebugUtilsMessengerEXT& messenger) const;

    private:
        friend class VulkanContext;

        template<typename T>
        void Load(char const* name, T& dst)
        {
            dst = reinterpret_cast<T>(m_context.GetInstanceProcAddr(m_vk_instance, name));

            if (dst == nullptr)
            {
                throw VulkanLoaderException(name);
            }
        }

        VulkanContext const& m_context;
        VkInstance m_vk_instance {VK_NULL_HANDLE};

        PFN_vkDestroyInstance m_vkDestroyInstance {nullptr};
        PFN_vkGetPhysicalDeviceProperties m_vkGetPhysicalDeviceProperties {nullptr};
        PFN_vkGetPhysicalDeviceMemoryProperties m_vkGetPhysicalDeviceMemoryProperties {nullptr};
        PFN_vkEnumeratePhysicalDevices m_vkEnumeratePhysicalDevices {nullptr};
        PFN_vkCreateWin32SurfaceKHR m_vkCreateWin32SurfaceKHR {nullptr};
        PFN_vkDestroySurfaceKHR m_vkDestroySurfaceKHR {nullptr};
        PFN_vkEnumerateDeviceExtensionProperties m_vkEnumerateDeviceExtensionProperties {nullptr};
        PFN_vkGetPhysicalDeviceQueueFamilyProperties m_vkGetPhysicalDeviceQueueFamilyProperties {nullptr};
        PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR m_vkGetPhysicalDeviceWin32PresentationSupportKHR {nullptr};
        PFN_vkGetPhysicalDeviceSurfaceSupportKHR m_vkGetPhysicalDeviceSurfaceSupportKHR {nullptr};
        PFN_vkCreateDevice m_vkCreateDevice {nullptr};
        PFN_vkCreateDebugUtilsMessengerEXT m_vkCreateDebugUtilsMessengerEXT {nullptr};
        PFN_vkDestroyDebugUtilsMessengerEXT m_vkDestroyDebugUtilsMessengerEXT {nullptr};
        PFN_vkGetPhysicalDeviceDescriptorSizeEXT m_vkGetPhysicalDeviceDescriptorSizeEXT {nullptr};
    };

} // Rc