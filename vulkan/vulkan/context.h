#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include "allocator.h"
#include "platform/library.h"
#include "error.h"
#include <string_view>
#include <span>
#include <memory>

namespace Rc
{
    class VulkanInstance;
    class VulkanDevice;

    struct VulkanVersion
    {
        uint32_t version {};

        uint32_t Major() const { return VK_VERSION_MAJOR(version); }
        uint32_t Minor() const { return VK_VERSION_MINOR(version); }
        uint32_t Patch() const { return VK_VERSION_PATCH(version); }
    };

    class VulkanContext
    {
    public:
        VulkanContext(std::string const& lib);
        ~VulkanContext() = default;

        VulkanContext(VulkanContext const&) = delete;
        VulkanContext(VulkanContext&&) = delete;
        VulkanContext& operator=(VulkanContext const&) = delete;
        VulkanContext& operator=(VulkanContext&&) = delete;

        PFN_vkVoidFunction GetInstanceProcAddr(VkInstance instance, const char* name) const
        {
            return m_vkGetInstanceProcAddr(instance, name);
        }

        PFN_vkVoidFunction GetDeviceProcAddr(VkDevice device, const char* name) const
        {
            return m_vkGetDeviceProcAddr(device, name);
        }

        VmaVulkanFunctions GetVmaFunctions(VulkanInstance const& instance, VulkanDevice const& device) const;

        // vkEnumerateInstanceVersion
        VulkanVersion EnumerateInstanceVersion() const;

        // vkEnumerateInstanceExtensionProperties
        uint32_t EnumerateInstanceExtensionPropertiesCount(std::string_view layer_name = {}) const;

        // vkEnumerateInstanceExtensionProperties
        std::span<VkExtensionProperties> EnumerateInstanceExtensionProperties(std::span<VkExtensionProperties> buffer, std::string_view layer_name = {}) const;
        
        // vkCreateInstance
        std::unique_ptr<VulkanInstance> CreateInstance(VkInstanceCreateInfo const& create_info) const;

    private:
        template<typename T>
        void Load(char const* name, T& dst)
        {
            dst = reinterpret_cast<T>(GetInstanceProcAddr(VK_NULL_HANDLE, name));

            if (dst == nullptr)
            {
                throw VulkanLoaderError(name);
            }
        }

        Library m_vklib;

        PFN_vkGetInstanceProcAddr m_vkGetInstanceProcAddr {nullptr};
        PFN_vkGetDeviceProcAddr m_vkGetDeviceProcAddr {nullptr};
        PFN_vkEnumerateInstanceVersion m_vkEnumerateInstanceVersion {nullptr};
        PFN_vkEnumerateInstanceExtensionProperties m_vkEnumerateInstanceExtensionProperties {nullptr};
        PFN_vkCreateInstance m_vkCreateInstance {nullptr};
        // vkEnumerateInstanceLayerProperties
    };

} // Rc