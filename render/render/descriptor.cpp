#include "descriptor.h"
#include <array>

namespace Rc::Render
{
    ResourceDescriptorBuilder::ResourceDescriptorBuilder(
        VulkanInstance const& instance,
        VulkanDevice const& device,
        VkPhysicalDevice vk_physical_device
    ) :
        m_vk_device{&device}
    {
        m_stride = instance.GetPhysicalDeviceDescriptorSizeEXT(vk_physical_device, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
    }

    // void ResourceDescriptorBuilder::Attach(std::shared_ptr<Buffer> host_buffer)
    // {
    //     m_buffer = std::move(host_buffer);
    // }

    void ResourceDescriptorBuilder::CreateUniformBuffer(uint32_t slot, Buffer const& buffer, Buffer& host_buffer, BufferRegion host_region)
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

        auto raw = host_buffer.Map(host_region.Offset() + (m_stride * slot), m_stride);

        VkHostAddressRangeEXT const range
        {
            .address = raw.data(),
            .size = static_cast<std::size_t>(m_stride)
        };

        m_vk_device->WriteResourceDescriptor(info, range);
    }
}