#pragma once

#include "vulkan/device.h"
#include "vulkan/instance.h"
#include "buffer.h"
#include <vector>

namespace Rc::Render
{
    struct UniformBufferDescriptor
    {
        uint64_t address;
        uint64_t size;
    };

    class ResourceDescriptor
    {
    public:
        ResourceDescriptor(UniformBufferDescriptor const& descriptor) :
            m_address{descriptor.address},
            m_size(descriptor.size),
            m_type{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER}
        {}

    private:
        friend class ResourceDescriptorHeap;

        uint64_t m_address {};
        uint64_t m_size {};
        VkDescriptorType m_type {};
    };

    class ResourceDescriptorHeap
    {
    public:
        ResourceDescriptorHeap(
            VulkanInstance const& instance,
            VulkanDevice const& device,
            VkPhysicalDevice vk_physical_device,
            std::span<ResourceDescriptor const> descriptors
        );

        // buffer - resource descriptor heap buffer
        void Attach(Buffer& descriptor_heap_buffer)
        {
            m_address = descriptor_heap_buffer.Address();
        }
        
        // Get required Buffer size
        uint64_t SizeTotal() const //------------------------------- rename
        {
            return m_reserved + m_buffer.size();
        }

        uint64_t Size() const //------------------------------- rename
        {
            return m_buffer.size();
        }

        uint64_t Reserved() const
        {
            return m_reserved;
        }

        uint64_t Address() const
        {
            return m_address;
        }

        void Write(std::span<std::byte> dst) const;

    private:
        VulkanDevice const* m_vk_device {nullptr};
        
        // Size of descriptor slot.
        std::size_t m_stride {0};

        uint64_t m_address {0};

        // Size of reserved region.
        uint64_t m_reserved {0};

        std::vector<std::byte> m_buffer;
    };

    // class SamplerDescriptorHeap

} // Rc::Render