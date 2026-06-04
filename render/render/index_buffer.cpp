#include "index_buffer.h"
#include <stdexcept>

namespace Rc::Render
{
    // // VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
    // // VMA_ALLOCATION_CREATE_MAPPED_BIT
    IndexBuffer::IndexBuffer(VmaAllocator vma_allocator, uint64_t size)
    {
        VmaAllocationCreateInfo alloc_info {};
        alloc_info.usage = VMA_MEMORY_USAGE_AUTO;

        VkBufferCreateInfo const buffer_info
        {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .size = size,
            .usage = VkBufferUsageFlags{
                VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                VK_BUFFER_USAGE_INDEX_BUFFER_BIT
            },
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = nullptr
        };

        Create(vma_allocator, alloc_info, buffer_info);
    }

} // Rc::Render