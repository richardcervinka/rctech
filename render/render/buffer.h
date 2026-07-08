#pragma once

#include "vulkan/device.h"
#include <span>
#include <cstddef>
#include <cassert>

namespace Rc::Render
{
    // GPU buffer region descriptor
    class BufferRegion
    {
    public:
        BufferRegion() = default;
        ~BufferRegion() = default;
        BufferRegion(BufferRegion const&) = default;
        BufferRegion& operator=(BufferRegion const&) = default;
        BufferRegion(BufferRegion&&) = default;
        BufferRegion& operator=(BufferRegion&&) = default;

        uint64_t Offset() const
        {
            return m_offset;
        }
        
        uint64_t Size() const
        {
            return m_size;
        }
        
        bool Empty() const
        {
            return m_size == 0;
        }

    private:
        friend class Buffer;

        BufferRegion(uint64_t offset, uint64_t size) :
            m_offset{offset},
            m_size{size}
        {}

        uint64_t m_offset {0};
        uint64_t m_size {0};
        //uint64_t m_generation {0};
    };

    struct VertexBufferInfo
    {
        uint64_t size;
    };

    struct IndexBufferInfo
    {
        uint64_t size;
    };

    struct StagingBufferInfo
    {
        uint64_t size;
    };

    struct UniformBufferInfo
    {
        uint64_t size;
    };

    struct DescriptorHeapBufferInfo
    {
        uint64_t size;
    };

    //
    // GPU buffer
    //
    class Buffer
    {
    public:
        
        Buffer(VulkanDevice const& vk_device, VertexBufferInfo const& info, VmaAllocator vma_allocator);
        Buffer(VulkanDevice const& vk_device, IndexBufferInfo const& info, VmaAllocator vma_allocator);
        Buffer(VulkanDevice const& vk_device, StagingBufferInfo const& info, VmaAllocator vma_allocator);
        Buffer(VulkanDevice const& vk_device, UniformBufferInfo const& info, VmaAllocator vma_allocator);
        Buffer(VulkanDevice const& vk_device, DescriptorHeapBufferInfo const& info, VmaAllocator vma_allocator);

        ~Buffer();

        Buffer(Buffer const&) = delete;
        Buffer& operator=(Buffer const&) = delete;
        Buffer(Buffer&& other) = delete;
        Buffer& operator=(Buffer&& other) = delete;

        VkBuffer Handle() const
        {
            return m_vk_buffer;
        }

        VkDeviceAddress Address() const;

        uint64_t Size() const
        {
            return m_vma_allocation_info.size;
        }

        BufferRegion GetRegion() const;
        BufferRegion GetRegion(uint64_t offset, uint64_t size) const;

        // Available only for stagging buffer.
        std::span<std::byte> Buffer::Map(uint64_t offset, uint64_t size);
        std::span<std::byte const> Buffer::Map(uint64_t offset, uint64_t size) const;

        std::span<std::byte> Buffer::Map()
        {
            return Map(0, Size());
        }

        std::span<std::byte> Buffer::Map(BufferRegion const& region)
        {
            return Map(region.Offset(), region.Size());
        }

        std::span<std::byte const> Buffer::Map(BufferRegion const& region) const
        {
            return Map(region.Offset(), region.Size());
        }

        // template<typename T>
        // std::span<T> Map(BufferRegion const& region)
        // {
        //     auto raw = m_buffer.Map(region);

        //     return {
        //         reinterpret_cast<T*>(raw.data()),
        //         region.Size() / sizeof(T)
        //     };
        // }

    private:
        friend class RenderCommandBuffer;
        friend class TransferCommandBuffer;

        Buffer(
            VulkanDevice const& vk_device,
            uint64_t size,
            VmaAllocator vma_allocator,
            VkBufferUsageFlags2 usage_flags,
            VmaAllocationCreateFlags vma_flags
        );

        VulkanDevice const* m_vk_device {nullptr};
        VkBuffer m_vk_buffer {VK_NULL_HANDLE};
        VmaAllocator m_vma_allocator {VK_NULL_HANDLE};
        VmaAllocation m_vma_allocation {nullptr};
        VmaAllocationInfo m_vma_allocation_info {};
        VkAccessFlags2 m_access_flags {VK_ACCESS_NONE};
        VkPipelineStageFlags m_stage_flags {VK_PIPELINE_STAGE_NONE};
    };

} // Rc::Render