#include "descriptor.h"
#include <array>

namespace Rc::Render
{
    ResourceDescriptorHeap::ResourceDescriptorHeap(
        VulkanInstance const& instance,
        VulkanDevice const& device,
        VkPhysicalDevice vk_physical_device
    ) :
        m_vk_device{&device}
    {
        auto const heap_properties = instance.GetPhysicalDeviceDescriptorHeapProperties(vk_physical_device);

        m_stride = heap_properties.bufferDescriptorSize;
        m_offset = heap_properties.minResourceHeapReservedRange;

        // heap_properties.minResourceHeapReservedRange
        m_buffer.resize(8 * m_stride); // ------------------------ 8 == number of slots
    }

    // void ResourceDescriptorBuilder::Attach(std::shared_ptr<Buffer> host_buffer)
    // {
    //     m_buffer = std::move(host_buffer);
    // }

    void ResourceDescriptorHeap::CreateUniformBuffer(uint32_t slot, Buffer const& buffer, BufferRegion region)
    {
        VkResourceDescriptorInfoEXT info
        {
            .sType = VK_STRUCTURE_TYPE_RESOURCE_DESCRIPTOR_INFO_EXT,
            .pNext = nullptr,
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
        };

        VkDeviceAddressRangeEXT const address_range
        {
            .address = buffer.Address(),
            .size = buffer.Size()
        };

        info.data.pAddressRange = &address_range;

        VkHostAddressRangeEXT const range
        {
            .address = m_buffer.data() + (m_stride * slot),
            .size = m_stride
        };

        m_vk_device->WriteResourceDescriptor(info, range);
    }
}