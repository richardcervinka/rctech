#pragma once

#include "vulkan/device.h"
#include "vulkan/instance.h"
#include "buffer.h"
#include <vector>

namespace Rc::Render
{
    // DescriptorFactory

    class ResourceDescriptorHeap // ----------------------- mel by take generovat layout pro pipeline state
    {
    public:

        ResourceDescriptorHeap(
            VulkanInstance const& instance,
            VulkanDevice const& device,
            VkPhysicalDevice vk_physical_device
        );

        // buffer - resource descriptor heap buffer
        void Attach(Buffer& buffer)
        {
            m_address = buffer.Address();
        }

        void CreateUniformBuffer(uint32_t slot, Buffer const& buffer, BufferRegion region);

        std::span<std::byte const> Data() const
        {
            return m_buffer;
        }

        // Get required Buffer size
        uint64_t SizeTotal() const //------------------------------- rename
        {
            return m_offset + m_buffer.size();
        }

        uint64_t Useful() const //------------------------------- rename
        {
            return m_buffer.size();
        }


        uint64_t Reserved() const
        {
            return m_offset;
        }

        uint64_t Address() const
        {
            return m_address;
        }

    private:
        VulkanDevice const* m_vk_device {nullptr};
        
        // Size of descriptor slot.
        std::size_t m_stride {0};

        uint64_t m_address {0};

        // Size of reserved region.
        uint64_t m_offset {0};

        //uint64_t m_alignment {0};

        std::vector<std::byte> m_buffer;
    };

    // class SamplerDescriptorHeap

} // Rc::Render