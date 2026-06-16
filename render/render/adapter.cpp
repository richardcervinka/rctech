#include "adapter.h"

namespace Rc::Render
{
    Adapter::Adapter(VulkanContext const& context, VulkanInstance const& instance, VkPhysicalDevice vk_physical_device) :
        m_context{&context},
        m_instance{&instance},
        m_vk_physical_device{vk_physical_device}
    {
        m_vk_properties = m_instance->GetPhysicalDeviceProperties(m_vk_physical_device);
        m_vk_memory_properties = m_instance->GetPhysicalDeviceMemoryProperties(m_vk_physical_device);
    }

    std::unique_ptr<Device> Adapter::CreateDevice(Surface const& surface)
    {
        return std::make_unique<Device>(*m_context, *m_instance, m_vk_physical_device, surface);
    }

    Version Adapter::ApiVersion() const
    {
        return
        {
            static_cast<int>(VK_VERSION_MAJOR(m_vk_properties.apiVersion)),
            static_cast<int>(VK_VERSION_MINOR(m_vk_properties.apiVersion))
        };
    }

    uint64_t Adapter::DeviceLocalMemory() const
    {
        uint64_t total = 0;

        for (uint32_t i = 0; i < m_vk_memory_properties.memoryHeapCount; i++)
        {
            auto const& heap = m_vk_memory_properties.memoryHeaps[i];

            if ((heap.flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) != 0)
            {
                total += heap.size;
            }
        }

        return total;
    }

} // Rc::Render