#pragma once

#include "vulkan/instance.h"
#include <string>
#include <map>
#include <memory>
#include <vector>
#include "swap_chain.h"
#include "device.h"
#include "surface.h"

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

        // Move-Only --------------------------------------- REVIEW
        Adapter(Adapter const&) = delete;
        Adapter& operator=(Adapter const&) = delete;
        Adapter(Adapter&& other) noexcept;
        Adapter& operator=(Adapter&& other) noexcept;

        uint32_t GetDeviceId() const { return m_vk_properties.deviceID; }

        std::string GetName() const { return m_vk_properties.deviceName; }

        bool IsIntegrated() const { return m_vk_properties.deviceType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU; }

        bool IsDiscrete() const { return m_vk_properties.deviceType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU; }

        std::unique_ptr<Device> CreateDevice(Surface const& surface);

    private:
        // Reference to a parent object.
        VulkanContext const* m_context {nullptr};
        VulkanInstance const* m_instance {nullptr};

        // Physical device handle.
        VkPhysicalDevice m_vk_physical_device {VK_NULL_HANDLE};
        
        VkPhysicalDeviceProperties m_vk_properties {};

        //vk::PhysicalDeviceMemoryProperties m_vk_memory_properties;
    };

} // Rc::Render