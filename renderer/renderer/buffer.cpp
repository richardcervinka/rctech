#include "buffer.h"

namespace Rc
{

Buffer::~Buffer()
{
    if (m_vma_allocator != VK_NULL_HANDLE)
    {
        vmaDestroyBuffer(m_vma_allocator, m_vk_buffer, m_vma_allocation);
    }
}

void Buffer::Create(
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

} // Rc