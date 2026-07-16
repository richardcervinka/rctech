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
        vk_device{device}
    {
        auto const heap_properties = instance.GetPhysicalDeviceDescriptorHeapProperties(vk_physical_device);

        stride = heap_properties.bufferDescriptorSize;
        reserved = heap_properties.minResourceHeapReservedRange;

        uint64_t buffer_size = descriptors.size() * stride;

        // Align size to VkPhysicalDeviceDescriptorHeapPropertiesEXT::resourceHeapAlignment
        // It is required for alignment of the reserved offset.
        if (uint64_t const tail = buffer_size % heap_properties.resourceHeapAlignment; tail > 0)
        {
            buffer_size += heap_properties.resourceHeapAlignment - tail;
        }
        
        buffer.resize(buffer_size);

        uint64_t slot = 0;

        for (auto const& descriptor : descriptors)
        {
            VkResourceDescriptorInfoEXT descriptor_info
            {
                .sType = VK_STRUCTURE_TYPE_RESOURCE_DESCRIPTOR_INFO_EXT,
                .pNext = nullptr,
                .type = descriptor.type
            };

            VkDeviceAddressRangeEXT const device_address_range
            {
                .address = descriptor.address,
                .size = descriptor.size
            };

            descriptor_info.data.pAddressRange = &device_address_range;

            VkHostAddressRangeEXT const host_address_range
            {
                .address = buffer.data() + (stride * slot),
                .size = stride
            };

            vk_device.WriteResourceDescriptor(descriptor_info, host_address_range);

            slot += 1;
        }
    }

    void ResourceDescriptorHeap::Write(std::span<std::byte> dst) const
    {
        std::copy(buffer.begin(), buffer.end(), dst.data());
    }
}