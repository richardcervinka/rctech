#pragma once

#include "vulkan/device.h"
#include <memory>
#include <span>
#include <cstddef>
#include <cassert>

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

    //
    // GPU buffer
    //
    class Buffer
    {
    public:
        
        Buffer(VertexBufferInfo const& info, VmaAllocator vma_allocator);
        Buffer(IndexBufferInfo const& info, VmaAllocator vma_allocator);
        Buffer(StagingBufferInfo const& info, VmaAllocator vma_allocator);

        ~Buffer();

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

        std::span<std::byte> Buffer::Data();
        std::span<std::byte const> Buffer::Data() const;

    private:
        friend class CommandBuffer;

        Buffer(
            uint64_t size,
            VmaAllocator vma_allocator,
            VkBufferUsageFlags usage,
            VmaAllocationCreateFlags vma_flags
        );

        VkBuffer m_vk_buffer {VK_NULL_HANDLE};
        VmaAllocator m_vma_allocator {VK_NULL_HANDLE};
        VmaAllocation m_vma_allocation {nullptr};
        VmaAllocationInfo m_vma_allocation_info {};
        VkAccessFlags m_access_flags {VK_ACCESS_NONE};
        VkPipelineStageFlags m_stage_flags {VK_PIPELINE_STAGE_NONE};
    };

    class LinearBuffer
    {
    public:
        LinearBuffer() = default;
        ~LinearBuffer() = default;

        explicit LinearBuffer(std::unique_ptr<Buffer> buffer) :
            m_buffer{std::move(buffer)}
        {}

        LinearBuffer(LinearBuffer const&) = delete;
        LinearBuffer& operator=(LinearBuffer const&) = delete;
        LinearBuffer(LinearBuffer&&) = default;
        LinearBuffer& operator=(LinearBuffer&&) = default;

        BufferRegion Pop(uint64_t size)
        {
            auto region = m_buffer->GetRegion(m_offset, size);
            m_offset += size;
            return region;
        }

        void Push(BufferRegion region)
        {     
        }

        Buffer& GetBuffer()
        {
            assert(m_buffer != nullptr);
            return *m_buffer;
        }

    private:
        std::unique_ptr<Buffer> m_buffer;
        uint64_t m_offset {0};
    };

    class BufferPool
    {
    public:

    private:
    };

} // Rc::Render