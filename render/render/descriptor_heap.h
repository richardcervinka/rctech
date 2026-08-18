#pragma once

#include "vulkan/device.h"
#include "vulkan/instance.h"
#include "buffer.h"
#include "texture.h"
#include <vector>

namespace Rc::Render
{
    // class ResourceBinding
    // {
    // public:

    // private:
    // };

    class ResourceDescriptorHeap
    {
    public:
        ResourceDescriptorHeap(
            VulkanInstance const& instance,
            VulkanDevice const& device,
            VkPhysicalDevice vk_physical_device,
            std::unique_ptr<Buffer> buffer
        );

        uint64_t Size() const
        {
            return buffer->Size();
        }

        uint64_t ReservedSize() const
        {
            return reserved_size;
        }

        uint64_t ReservedOffset() const
        {
            return reserved_offset;
        }

        uint64_t Address() const
        {
            return address;
        }

        BufferRegion GetBufferRegion() const
        {
            return buffer->GetRegion();
        }

        void WriteUniformBufferDescriptor(
            uint64_t index,
            uint64_t address,
            uint64_t size
        );
        
        void WriteTexture2dDescriptor(
            uint64_t index,
            Texture2d const& texture
        );

        void Write(std::span<std::byte> dst) const;

    private:
        VulkanDevice const& vk_device;
        
        // Size of descriptor slot.
        uint64_t buffer_descriptor_size {0};
        uint64_t image_descriptor_size {0};

        uint64_t address {0};

        // Size of reserved region.
        uint64_t reserved_offset {0};
        uint64_t reserved_size {0};

        uint32_t begin_buffer_index {0};
        uint32_t end_buffer_index {0};
        uint32_t begin_texture_index {0};
        uint32_t end_texture_index {0};

        std::vector<std::byte> data;

        std::unique_ptr<Buffer> buffer;
    };

    // class SamplerDescriptorHeap

} // Rc::Render