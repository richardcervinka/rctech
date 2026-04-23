#include "context.h"
#include "instance.h"
#include "device.h"

namespace Rc
{
    VulkanContext::VulkanContext(std::string const& lib)
    {
        m_vklib = Library(lib);
        m_vkGetInstanceProcAddr = m_vklib.GetPfn<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
        m_vkGetDeviceProcAddr = m_vklib.GetPfn<PFN_vkGetDeviceProcAddr>("vkGetDeviceProcAddr");

        Load("vkEnumerateInstanceVersion", m_vkEnumerateInstanceVersion);
        Load("vkEnumerateInstanceExtensionProperties", m_vkEnumerateInstanceExtensionProperties);
        Load("vkCreateInstance", m_vkCreateInstance);
    }

    VmaVulkanFunctions VulkanContext::GetVmaFunctions(VulkanInstance const& instance, VulkanDevice const& device) const
    {
        VmaVulkanFunctions functions {};
        functions.vkGetInstanceProcAddr = m_vkGetInstanceProcAddr;
        functions.vkGetDeviceProcAddr = m_vkGetDeviceProcAddr;
        functions.vkGetPhysicalDeviceProperties = instance.m_vkGetPhysicalDeviceProperties;
        functions.vkGetPhysicalDeviceMemoryProperties = instance.m_vkGetPhysicalDeviceMemoryProperties;
        // functions.vkAllocateMemory;
        // functions.vkFreeMemory;
        // functions.vkMapMemory;
        // functions.vkUnmapMemory;
        // functions.vkFlushMappedMemoryRanges;
        // functions.vkInvalidateMappedMemoryRanges;
        // functions.vkBindBufferMemory;
        // functions.vkBindImageMemory;
        // functions.vkGetBufferMemoryRequirements;
        // functions.vkGetImageMemoryRequirements;
        // functions.vkCreateBuffer
        // functions.vkDestroyBuffer;
        // functions.vkCreateImage;
        // functions.vkDestroyImage;
        functions.vkCmdCopyBuffer = device.m_vkCmdCopyBuffer;

        return functions;
    }

    VulkanVersion VulkanContext::EnumerateInstanceVersion() const
    {
        uint32_t result {};
        if (auto vk_result = m_vkEnumerateInstanceVersion(&result); vk_result != VK_SUCCESS)
        {
            throw VulkanException(vk_result);
        }
        return {result};
    }

    uint32_t VulkanContext::EnumerateInstanceExtensionPropertiesCount(std::string_view layer_name) const
    {
        uint32_t result = 0;
        if (auto vk_result = m_vkEnumerateInstanceExtensionProperties(layer_name.data(), &result, nullptr); vk_result != VK_SUCCESS)
        {
            throw VulkanException(vk_result);
        }
        return result;
    }

    std::span<VkExtensionProperties> VulkanContext::EnumerateInstanceExtensionProperties(std::span<VkExtensionProperties> buffer, std::string_view layer_name) const
    {
        auto count = static_cast<uint32_t>(buffer.size());
        if (auto vk_result = m_vkEnumerateInstanceExtensionProperties(layer_name.data(), &count, buffer.data()); vk_result != VK_SUCCESS)
        {
            throw VulkanException(vk_result);
        }
        return buffer.subspan(0, count);
    }

    std::unique_ptr<VulkanInstance> VulkanContext::CreateInstance(VkInstanceCreateInfo const& create_info) const
    {
        VkInstance vk_instance {VK_NULL_HANDLE};

        if (auto vk_result = m_vkCreateInstance(&create_info, nullptr, &vk_instance); vk_result != VK_SUCCESS)
        {
            throw VulkanException(vk_result);
        }

        return std::make_unique<VulkanInstance>(*this, vk_instance);
    }

} // Rc