#include "descriptor_heap.h"
#include <array>

namespace Rc::Render
{
    // ResourceDescriptorHeap
    
    ResourceDescriptorHeap::ResourceDescriptorHeap(
        VulkanInstance const& instance,
        VulkanDevice const& device,
        VkPhysicalDevice vk_physical_device,
        std::unique_ptr<Buffer> buffer
    ) :
        vk_device{device}
    {
        auto const heap_properties = instance.GetPhysicalDeviceDescriptorHeapProperties(vk_physical_device);

        assert(buffer->Size() >= heap_properties.minResourceHeapReservedRange);

        buffer_descriptor_size = heap_properties.bufferDescriptorSize;
        image_descriptor_size = heap_properties.imageDescriptorSize;
        reserved_size = heap_properties.minResourceHeapReservedRange;
        reserved_offset = buffer->Size() - heap_properties.minResourceHeapReservedRange;

        // Align size to VkPhysicalDeviceDescriptorHeapPropertiesEXT::resourceHeapAlignment
        // It is required for alignment of the reserved offset.
        if (uint64_t const tail = reserved_offset % heap_properties.resourceHeapAlignment; tail > 0)
        {
            reserved_offset -= tail;
        }

        begin_buffer_index = 0;
        end_buffer_index = 8;

        // begin_texture_index
        // end_texture_index
        {
            auto offset = end_buffer_index * buffer_descriptor_size;

            // Align offset to the image_descriptor_size
            if (uint64_t const tail = offset % image_descriptor_size; tail > 0)
            {
                offset += (image_descriptor_size - tail);
            }

            begin_texture_index = offset / image_descriptor_size;
            end_texture_index = begin_texture_index + ((reserved_offset - offset) / image_descriptor_size);
        }
        
        data.resize(reserved_offset);
        this->buffer = std::move(buffer);
        address = this->buffer->Address();
    }

    void ResourceDescriptorHeap::WriteUniformBufferDescriptor(
        uint64_t index,
        uint64_t address,
        uint64_t size)
    {
        assert(index >= begin_buffer_index);
        assert(index < end_buffer_index);

        VkDeviceAddressRangeEXT const device_address
        {
            .address = address,
            .size = size
        };

        VkResourceDescriptorInfoEXT const resource_descriptor_info
        {
            .sType = VK_STRUCTURE_TYPE_RESOURCE_DESCRIPTOR_INFO_EXT,
            .pNext = nullptr,
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .data.pAddressRange = &device_address
        };

        VkHostAddressRangeEXT const host_address
        {
            .address = data.data() + (buffer_descriptor_size * index),
            .size = buffer_descriptor_size
        };

        vk_device.WriteResourceDescriptorEXT(resource_descriptor_info, host_address);
    }

    void ResourceDescriptorHeap::WriteTexture2dDescriptor(
        uint64_t index,
        Texture2d const& texture)
    {
        assert(index >= begin_texture_index);
        assert(index < end_texture_index);

        VkImageViewCreateInfo image_view_info
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .image = texture.Underlying(),
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = texture.UnderlyingFormat(),
            .components = {
                VK_COMPONENT_SWIZZLE_IDENTITY,
                VK_COMPONENT_SWIZZLE_IDENTITY,
                VK_COMPONENT_SWIZZLE_IDENTITY,
                VK_COMPONENT_SWIZZLE_IDENTITY
            },
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        };

        VkImageDescriptorInfoEXT const image_descriptor_info
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_DESCRIPTOR_INFO_EXT,
            .pNext = nullptr,
            .pView = &image_view_info,
            .layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        };

        VkResourceDescriptorInfoEXT const resource_descriptor_info
        {
            .sType = VK_STRUCTURE_TYPE_RESOURCE_DESCRIPTOR_INFO_EXT,
            .pNext = nullptr,
            .type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
            .data.pImage = &image_descriptor_info
        };

        VkHostAddressRangeEXT const host_address
        {
            .address = data.data() + (image_descriptor_size * index),
            .size = image_descriptor_size
        };

        vk_device.WriteResourceDescriptorEXT(resource_descriptor_info, host_address);
    }

    // void ResourceDescriptorHeap::Write(std::span<std::byte> dst) const
    // {
    //     std::copy(data.begin(), data.end(), dst.data());
    // }

    // SamplerDescriptorHeap

    SamplerDescriptorHeap::SamplerDescriptorHeap(
        VulkanInstance const& instance,
        VulkanDevice const& device,
        VkPhysicalDevice vk_physical_device,
        std::unique_ptr<Buffer> buffer
    ) :
        vk_device{device}
    {
        auto const heap_properties = instance.GetPhysicalDeviceDescriptorHeapProperties(vk_physical_device);

        assert(buffer->Size() >= heap_properties.minSamplerHeapReservedRange);

        descriptor_size = heap_properties.samplerDescriptorSize;
        reserved_size = heap_properties.minSamplerHeapReservedRange;
        reserved_offset = buffer->Size() - heap_properties.minSamplerHeapReservedRange;

        // Align size to VkPhysicalDeviceDescriptorHeapPropertiesEXT::resourceHeapAlignment
        // It is required for alignment of the reserved offset.
        if (uint64_t const tail = reserved_offset % heap_properties.samplerHeapAlignment; tail > 0)
        {
            reserved_offset -= tail;
        }

        begin_index = 0;
        end_index = reserved_offset % descriptor_size;
        
        data.resize(reserved_offset);
        this->buffer = std::move(buffer);
        address = this->buffer->Address();
    }

    void SamplerDescriptorHeap::WriteDefaultSampler(uint64_t index)
    {
        VkSamplerCreateInfo sampler_info
        {
            .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            .magFilter = VK_FILTER_LINEAR,
            .minFilter = VK_FILTER_LINEAR,
            .mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST,
            .minLod = 0.0f,
            .maxLod = 0.0f,
            .mipLodBias = 0.0f,
            .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .anisotropyEnable = VK_FALSE,
            .maxAnisotropy = 1.0f,
            .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
            .unnormalizedCoordinates = VK_FALSE
        };

        VkHostAddressRangeEXT const host_address
        {
            .address = data.data() + (descriptor_size * index),
            .size = descriptor_size
        };

        vk_device.WriteSamplerDescriptorEXT(sampler_info, host_address);
    }
}