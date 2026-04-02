#include "vertex_buffer.h"
#include <algorithm>
#include "error.h"

namespace Rc
{
    // // VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
    // // VMA_ALLOCATION_CREATE_MAPPED_BIT
    VertexBuffer::VertexBuffer(VmaAllocator vma_allocator, std::size_t size)
    {
        VmaAllocationCreateInfo alloc_info {};
        alloc_info.usage = VMA_MEMORY_USAGE_AUTO;

        VkBufferCreateInfo buffer_info
        {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .size = size,
            .usage = {
                VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
            },
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

    VertexBuffer::~VertexBuffer()
    {
        if (m_vma_allocator != VK_NULL_HANDLE)
        {
            vmaDestroyBuffer(m_vma_allocator, m_vk_buffer, m_vma_allocation);
        }
    }

} // Rc