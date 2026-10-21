#pragma once

#include "vulkan/device.h"
#include "vulkan/instance.h"
#include "buffer.h"
#include "texture.h"
#include <vector>

namespace Rc::Render
{
    class ResourceDescriptorHeap
    {
    public:
        ResourceDescriptorHeap(
            VulkanInstance const& instance,
            VulkanDevice const& device,
            VkPhysicalDevice vk_physical_device,
            std::unique_ptr<Buffer> buffer
        );

        // Descriptor heap size in bytes.
        uint64_t Size() const
        {
            return buffer->Size();
        }

        // Number of bytes reserved for driver internal usage.
        uint64_t ReservedSize() const
        {
            return reserved_size;
        }

        // Offset in bytes of the block reserved for driver internal usage.
        uint64_t ReservedOffset() const
        {
            return reserved_offset;
        }

        // GPU address.
        uint64_t Address() const
        {
            return address;
        }

        BufferRegion GetBufferRegion() const
        {
            return buffer->GetRegion(0, reserved_offset);
        }

        // Index is a descriptor shader index.
        void WriteUniformBufferDescriptor(
            uint64_t index, //------------------------------------- 64 ?
            uint64_t address,
            uint64_t size  // --------------------------------------- 64 ?
        );
        
        // Index is a descriptor shader index.
        void WriteTexture2DDescriptor(
            uint64_t index,
            Texture2D const& texture
        );

        BufferRegion GetTexture2DBufferRegion(uint64_t index) const
        {
            return buffer->GetRegion(image_descriptor_size * index, image_descriptor_size);
        }

        // Host-visible descriptor heap memory.
        std::span<std::byte const> Data() const
        {
            return data;
        }

        // Convert logical resource slot to a descriptor index.
        uint32_t TranslateTextureSlot(uint32_t slot) const //----------------------------- TextureSlot ?
        {
            return slot + begin_texture_index;
        }

        // void Write(std::span<std::byte> dst) const; // ----------------- Rename to Copy()

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

    struct Sampler {
        // VkFilter                magFilter;
        // VkFilter                minFilter;
        // VkSamplerMipmapMode     mipmapMode;
        // VkSamplerAddressMode    addressModeU;
        // VkSamplerAddressMode    addressModeV;
        // VkSamplerAddressMode    addressModeW;
        float mip_lod_bias;
        bool anisotropy_enable;
        float max_anisotropy;
        bool compare_enable;
        // VkCompareOp             compareOp;
        float min_lod;
        float max_lod;
        bool border_color;
        bool unnormalized_coordinates;
    };

    // VkSamplerCreateInfo samplerInfo{
    //     .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
    //     .magFilter = VK_FILTER_LINEAR,
    //     .minFilter = VK_FILTER_LINEAR,
    //
    //     // Bez mipmap
    //     .mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST,
    //     .minLod = 0.0f,
    //     .maxLod = 0.0f,
    //     .mipLodBias = 0.0f,
    //
    //     // Adresování
    //     .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
    //     .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
    //     .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
    //
    //     // Anizotropie vypnutá (nemá smysl bez mipů)
    //     .anisotropyEnable = VK_FALSE,
    //     .maxAnisotropy = 1.0f,
    //
    //     // Border color jen když používáš CLAMP_TO_BORDER
    //     .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
    //
    //     // Normalized coords (typické)
    //     .unnormalizedCoordinates = VK_FALSE
    // };

    class SamplerDescriptorHeap
    {
    public:
        SamplerDescriptorHeap(
            VulkanInstance const& instance,
            VulkanDevice const& device,
            VkPhysicalDevice vk_physical_device,
            std::unique_ptr<Buffer> buffer
        );

        void WriteDefaultSampler(uint64_t index);

        // Descriptor heap size in bytes.
        uint64_t Size() const
        {
            return buffer->Size();
        }

        // Number of bytes reserved for driver internal usage.
        uint64_t ReservedSize() const
        {
            return reserved_size;
        }

        // Offset in bytes of the block reserved for driver internal usage.
        uint64_t ReservedOffset() const
        {
            return reserved_offset;
        }

        // GPU address.
        uint64_t Address() const
        {
            return address;
        }

        // Host-visible descriptor heap memory.
        std::span<std::byte const> Data() const
        {
            return data;
        }
        
        BufferRegion GetBufferRegion() const
        {
            return buffer->GetRegion(0, reserved_offset);
        }

        void Write(std::span<std::byte> dst) const;

    private:
        VulkanDevice const& vk_device;
        
        // Size of descriptor slot.
        uint64_t descriptor_size {0};

        uint64_t address {0};

        // Size of reserved region.
        uint64_t reserved_offset {0};
        uint64_t reserved_size {0};

        uint32_t begin_index {0};
        uint32_t end_index {0};

        std::vector<std::byte> data;

        std::unique_ptr<Buffer> buffer;
    };

} // Rc::Render