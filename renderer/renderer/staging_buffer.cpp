#include "staging_buffer.h"
#include <algorithm>
#include "error.h"

namespace Rc
{
    StagingBuffer::StagingBuffer(VmaAllocator vma_allocator, std::size_t size)
    {
        VmaAllocationCreateInfo alloc_info {};
        alloc_info.usage = VMA_MEMORY_USAGE_AUTO;
        alloc_info.flags = {
            VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
            VMA_ALLOCATION_CREATE_MAPPED_BIT
        };

        VkBufferCreateInfo buffer_info
        {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .size = size,
            .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = nullptr
        };
        
        auto const vk_result = vmaCreateBuffer(
            vma_allocator,
            &buffer_info,
            &alloc_info,
            &m_vk_buffer,
            &m_vma_allocation,
            &m_vma_allocation_info
        );

        if (vk_result != VK_SUCCESS)
        {
            throw VulkanError(vk_result);
        }

        m_vma_allocator = vma_allocator;
    }

    StagingBuffer::~StagingBuffer()
    {
        if (m_vma_allocator != VK_NULL_HANDLE)
        {
            vmaDestroyBuffer(m_vma_allocator, m_vk_buffer, m_vma_allocation);
        }
    }


    std::span<std::byte> StagingBuffer::Data()
    {
        return {
            static_cast<std::byte*>(m_vma_allocation_info.pMappedData),
            static_cast<std::size_t>(m_vma_allocation_info.size)
        };
    }

    // void* data = nullptr;

    // if (auto vk_result = vmaMapMemory(m_vma_allocator, m_vma_allocation, &data); vk_result != VK_SUCCESS)
    // {
    //     throw VulkanError(vk_result);
    // }

    // m_map = {static_cast<std::byte*>(data), static_cast<std::size_t>(m_vma_allocation_info.size)};

} // Rc