#pragma once

#include "vulkan/device.h"
#include <memory>
#include <span>
#include <cstddef>

namespace Rc
{
    class VertexBuffer
    {
    public:
        VertexBuffer(VmaAllocator vma_allocator, std::size_t size);

        VertexBuffer(const VertexBuffer&) = delete;
        VertexBuffer& operator=(const VertexBuffer&) = delete;
        VertexBuffer(VertexBuffer&& other) = delete;
        VertexBuffer& operator=(VertexBuffer&& other) = delete;

        ~VertexBuffer();

        uint64_t Size() const { return m_vma_allocation_info.size; }

    private:
        friend class CommandBuffer;

        VmaAllocator m_vma_allocator {VK_NULL_HANDLE};
        VkBuffer m_vk_buffer {VK_NULL_HANDLE};
        VmaAllocation m_vma_allocation;
        VmaAllocationInfo m_vma_allocation_info {};
    };

} // Rc
