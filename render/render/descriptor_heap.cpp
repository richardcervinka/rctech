#include "descriptor_heap.h"
#include <array>

namespace Rc::Render
{
    ResourceDescriptorHeap::ResourceDescriptorHeap(
        VulkanInstance const& instance,
        VulkanDevice const& device,
        VkPhysicalDevice vk_physical_device,
        std::span<ResourceDescriptor const> descriptors
    ) :
        m_vk_device{&device}
    {
        auto const heap_properties = instance.GetPhysicalDeviceDescriptorHeapProperties(vk_physical_device);

        m_stride = heap_properties.bufferDescriptorSize;
        m_reserved = heap_properties.minResourceHeapReservedRange;

        uint64_t buffer_size = descriptors.size() * m_stride;

        // Align size to VkPhysicalDeviceDescriptorHeapPropertiesEXT::resourceHeapAlignment
        // It is required for alignment of the reserved offset.
        if (uint64_t const tail = buffer_size % heap_properties.resourceHeapAlignment; tail > 0)
        {
            buffer_size += heap_properties.resourceHeapAlignment - tail;
        }
        
        m_buffer.resize(buffer_size);

        uint64_t slot = 0;

        for (auto const& descriptor : descriptors)
        {
            VkResourceDescriptorInfoEXT descriptor_info
            {
                .sType = VK_STRUCTURE_TYPE_RESOURCE_DESCRIPTOR_INFO_EXT,
                .pNext = nullptr,
                .type = descriptor.m_type
            };

            VkDeviceAddressRangeEXT const device_address_range
            {
                .address = descriptor.m_address,
                .size = descriptor.m_size
            };

            descriptor_info.data.pAddressRange = &device_address_range;

            VkHostAddressRangeEXT const host_address_range
            {
                .address = m_buffer.data() + (m_stride * slot),
                .size = m_stride
            };

            m_vk_device->WriteResourceDescriptor(descriptor_info, host_address_range);

            slot += 1;
        }
    }

    void ResourceDescriptorHeap::Write(std::span<std::byte> dst) const
    {
        std::copy(m_buffer.begin(), m_buffer.end(), dst.data());
    }
}