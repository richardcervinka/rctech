#pragma once

#include "vulkan/device.h"
#include <memory>
#include <span>
#include <cstddef>

namespace Rc::Render
{
    class BufferRegion
    {
    public:
        BufferRegion() = default;
        ~BufferRegion() = default;
        BufferRegion(BufferRegion const&) = default;
        BufferRegion& operator=(BufferRegion const&) = default;
        BufferRegion(BufferRegion&&) = default;
        BufferRegion& operator=(BufferRegion&&) = default;

        uint64_t GetOffset() const
        {
            return m_offset;
        }
        
        uint64_t GetSize() const
        {
            return m_size;
        }
        
        bool Empty() const
        {
            return m_size == 0;
        }

    private:
        friend class Buffer;

        BufferRegion(uint64_t offset, uint64_t size) : m_offset{offset}, m_size{size} {}

        uint64_t m_offset {0};
        uint64_t m_size {0};
    };

    class Buffer
    {
    public:
        Buffer() = default;
        virtual ~Buffer();

        Buffer(Buffer const&) = delete;
        Buffer& operator=(Buffer const&) = delete;
        Buffer(Buffer&& other) = delete;
        Buffer& operator=(Buffer&& other) = delete;

        VkBuffer Handle() const
        {
            return m_vk_buffer;
        }

        uint64_t Size() const
        {
            return m_vma_allocation_info.size;
        }

        BufferRegion GetRegion(uint64_t offset, uint64_t size) const;

    protected:
        void Create(
            VmaAllocator vma_allocator,
            VmaAllocationCreateInfo const& alloc_info,
            VkBufferCreateInfo const& buffer_info
        );

        friend class CommandBuffer;

        VkBuffer m_vk_buffer {VK_NULL_HANDLE};
        VmaAllocator m_vma_allocator {VK_NULL_HANDLE};
        VmaAllocation m_vma_allocation {nullptr};
        VmaAllocationInfo m_vma_allocation_info {};
        VkAccessFlags m_access_flags {VK_ACCESS_NONE};
        VkPipelineStageFlags m_stage_flags {VK_PIPELINE_STAGE_NONE};
    };

} // Rc::Render