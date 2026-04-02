#include "adapter.h"

namespace Rc
{
    Adapter::Adapter(VulkanContext const& context, VulkanInstance const& instance, VkPhysicalDevice vk_physical_device) :
        m_context{&context},
        m_instance{&instance},
        m_vk_physical_device{vk_physical_device}
    {
        m_vk_properties = m_instance->GetPhysicalDeviceProperties(m_vk_physical_device);
        //m_vk_memory_properties = m_vk_physical_device.getMemoryProperties();
    }

    Adapter::Adapter(Adapter&& other) noexcept
    {
        std::swap(m_context, other.m_context);
        std::swap(m_instance, other.m_instance);
        std::swap(m_vk_physical_device, other.m_vk_physical_device);
        m_vk_properties = other.m_vk_properties;
    }

    Adapter& Adapter::operator=(Adapter&& other) noexcept
    {
        std::swap(m_context, other.m_context);
        std::swap(m_instance, other.m_instance);
        std::swap(m_vk_physical_device, other.m_vk_physical_device);
        m_vk_properties = other.m_vk_properties;
        return *this;
    }

    std::unique_ptr<Device> Adapter::CreateDevice(Surface const& surface)
    {
        return std::make_unique<Device>(*m_context, *m_instance, m_vk_physical_device, surface);
    }

    // adapter.m_vk_device.enumerateDeviceExtensionProperties

} // Rc