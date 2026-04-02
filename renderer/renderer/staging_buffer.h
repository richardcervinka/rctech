#pragma once

#include "vulkan/device.h"
#include <memory>
#include <span>
#include <cstddef>

namespace Rc
{
    class StagingBuffer
    {
    public:
        StagingBuffer(VmaAllocator vma_allocator, std::size_t size);

        ~StagingBuffer();

        StagingBuffer(const StagingBuffer&) = delete;
        StagingBuffer& operator=(const StagingBuffer&) = delete;
        StagingBuffer(StagingBuffer&& other) = delete;
        StagingBuffer& operator=(StagingBuffer&& other) = delete;

        VkBuffer Handle() const { return m_vk_buffer; }

        uint64_t Size() const { return m_vma_allocation_info.size; }

        std::span<std::byte> Data();
        //std::span<std::byte> Map(); // -------------- unmap?

    private:
        friend class CommandBuffer; // --------------

        VmaAllocator m_vma_allocator {VK_NULL_HANDLE};
        VkBuffer m_vk_buffer {VK_NULL_HANDLE};
        VmaAllocation m_vma_allocation {nullptr};
        VmaAllocationInfo m_vma_allocation_info {};
    };

} // Rc