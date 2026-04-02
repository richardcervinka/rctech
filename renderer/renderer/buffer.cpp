#include "buffer.h"
#include <algorithm>
#include "error.h"

namespace Rc
{
    Buffer::Buffer(
        VmaAllocator vma_allocator,
        VmaAllocationCreateInfo const& alloc_info,
        VkBufferCreateInfo const& buffer_info)
    {
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

    Buffer::Buffer(Buffer&& other)
    {
        std::swap(m_vma_allocator, other.m_vma_allocator);
        std::swap(m_vk_buffer, other.m_vk_buffer);
        m_vma_allocation = other.m_vma_allocation;
        m_vma_allocation_info = other.m_vma_allocation_info;
    }

    Buffer& Buffer::operator=(Buffer&& other)
    {
        std::swap(m_vma_allocator, other.m_vma_allocator);
        std::swap(m_vk_buffer, other.m_vk_buffer);
        m_vma_allocation = other.m_vma_allocation;
        m_vma_allocation_info = other.m_vma_allocation_info;
        return *this;
    }

    Buffer::~Buffer()
    {
        if (m_vma_allocator != VK_NULL_HANDLE)
        {
            vmaDestroyBuffer(m_vma_allocator, m_vk_buffer, m_vma_allocation);
            m_vma_allocator = VK_NULL_HANDLE;
        }
    }

    // std::span<std::byte> Buffer::Map()
    // {
    //     // vmaMapMemory(_allocator, mesh._vertexBuffer._allocation, &data);
    //     void* data = nullptr;
    //     vmaMapMemory(m_vma_allocator, m_vma_allocation, &data); // ---------------------- VkResult

    //     return {static_cast<std::byte*>(data), static_cast<std::size_t>(m_vma_allocation_info.size)};
    // }

    std::span<std::byte> Buffer::Data()
    {
        return {
            static_cast<std::byte*>(m_vma_allocation_info.pMappedData),
            static_cast<std::size_t>(m_vma_allocation_info.size)
        };
    }

} // Rc