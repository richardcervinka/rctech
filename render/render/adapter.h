#pragma once

#include "vulkan/instance.h"
#include <string>
#include <memory>
#include "device.h"
#include "surface.h"
#include "version.h"

namespace Rc::Render
{
    //
    // Physical device (graphics adapter).
    //
    class Adapter
    {
    public:

        Adapter() = default;

        Adapter(VulkanContext const& context, VulkanInstance const& instance, VkPhysicalDevice vk_physical_device);

        uint32_t DeviceId() const
        {
            return m_vk_properties.deviceID;
        }

        std::string Name() const
        {
            return m_vk_properties.deviceName;
        }

        bool Integrated() const
        {
            return m_vk_properties.deviceType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;
        }

        bool Discrete() const
        {
            return m_vk_properties.deviceType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
        }

        Version ApiVersion() const;

        uint64_t DeviceLocalMemory() const;

        std::unique_ptr<Device> CreateDevice(Surface const& surface);

    private:
        // Reference to a parent object.
        VulkanContext const* m_context {nullptr};
        VulkanInstance const* m_instance {nullptr};

        // Physical device handle.
        VkPhysicalDevice m_vk_physical_device {VK_NULL_HANDLE};
        
        VkPhysicalDeviceProperties m_vk_properties {};
        VkPhysicalDeviceMemoryProperties m_vk_memory_properties {};
    };

} // Rc::Render