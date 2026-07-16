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
            address{descriptor.address},
            size(descriptor.size),
            type{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER}
        {}

    private:
        friend class ResourceDescriptorHeap;

        uint64_t address {};
        uint64_t size {};
        VkDescriptorType type {};
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
            address = descriptor_heap_buffer.Address();
        }
        
        // Get required Buffer size
        uint64_t SizeTotal() const //------------------------------- rename
        {
            return reserved + buffer.size();
        }

        uint64_t Size() const //------------------------------- rename
        {
            return buffer.size();
        }

        uint64_t Reserved() const
        {
            return reserved;
        }

        uint64_t Address() const
        {
            return address;
        }

        void Write(std::span<std::byte> dst) const;

    private:
        VulkanDevice const& vk_device;
        
        // Size of descriptor slot.
        std::size_t stride {0};

        uint64_t address {0};

        // Size of reserved region.
        uint64_t reserved {0};

        std::vector<std::byte> buffer;
    };

    // class SamplerDescriptorHeap

} // Rc::Render