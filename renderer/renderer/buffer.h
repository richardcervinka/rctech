#pragma once

#include "vulkan/device.h"
#include <memory>
#include <span>
#include <cstddef>

namespace Rc
{
    class Buffer
    {
    public:
        Buffer(
            VmaAllocator vma_allocator,
            VmaAllocationCreateInfo const& alloc_info,
            VkBufferCreateInfo const& buffer_info
        );

        ~Buffer();

        // Move-Only
        Buffer(const Buffer&) = delete;
        Buffer& operator=(const Buffer&) = delete;
        Buffer(Buffer&& other) = delete;
        Buffer& operator=(Buffer&& other) = delete;

        uint64_t Size() const { return m_vma_allocation_info.size; }

        std::span<std::byte> Data();
        // std::span<std::byte> Map();

    private:
        friend class CommandBuffer;

        VmaAllocator m_vma_allocator {VK_NULL_HANDLE};
        VkBuffer m_vk_buffer {VK_NULL_HANDLE};
        VmaAllocation m_vma_allocation;
        VmaAllocationInfo m_vma_allocation_info {};
    };

} // Rc