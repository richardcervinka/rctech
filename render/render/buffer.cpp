#include "buffer.h"
#include <stdexcept>
#include <cassert>

namespace Rc::Render
{
    Buffer::~Buffer()
    {
        if (vma_allocator != VK_NULL_HANDLE)
        {
            vmaDestroyBuffer(vma_allocator, vk_buffer, vma_allocation);
        }
    }
    
    Buffer::Buffer(
        VulkanDevice const& vk_device,
        uint64_t size,
        VmaAllocator vma_allocator,
        VkBufferUsageFlags2 usage_flags,
        VmaAllocationCreateFlags vma_flags
    ) :
        vk_device{vk_device}
    {
        VmaAllocationCreateInfo alloc_info {};
        alloc_info.usage = VMA_MEMORY_USAGE_AUTO;
        alloc_info.flags = vma_flags;

        VkBufferUsageFlags2CreateInfo const create_info_2
        {
            .sType = VK_STRUCTURE_TYPE_BUFFER_USAGE_FLAGS_2_CREATE_INFO,
            .pNext = nullptr,
            .usage = usage_flags
        };

        VkBufferCreateInfo create_info
        {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .pNext = &create_info_2,
            .flags = 0,
            .size = size,
            .usage =  0,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = nullptr
        };

        auto const vk_result = vmaCreateBuffer(
            vma_allocator,
            &create_info,
            &alloc_info,
            &vk_buffer,
            &vma_allocation,
            &vma_allocation_info
        );

        if (vk_result != VK_SUCCESS)
        {
            throw VulkanException(vk_result);
        }

        this->vma_allocator = vma_allocator;   
    }

    BufferRegion Buffer::GetRegion() const
    {
        return {vk_buffer, 0, Size()};
    }

    BufferRegion Buffer::GetRegion(uint64_t offset, uint64_t size) const
    {
        if (offset + size > Size())
        {
            throw std::runtime_error("REFACTOR ME"); //--------------------------
        }

        return {vk_buffer, offset, size};
    }

    std::span<std::byte> Buffer::Map(uint64_t offset, uint64_t size)
    {
        assert(offset + size <= vma_allocation_info.size);

        return {
            static_cast<std::byte*>(vma_allocation_info.pMappedData) + offset,
            static_cast<std::size_t>(size)
        };
    }

    std::span<std::byte const> Buffer::Map(uint64_t offset, uint64_t size) const
    {
        assert(offset + size <= vma_allocation_info.size);

        return {
            static_cast<std::byte const*>(vma_allocation_info.pMappedData) + offset,
            static_cast<std::size_t>(size)
        };
    }

    VkDeviceAddress Buffer::Address() const
    {
        VkBufferDeviceAddressInfo const info
        {
            .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
            .pNext = nullptr,
            .buffer = vk_buffer
        };

        return vk_device.GetBufferDeviceAddress(info);
    }

} // Rc::Render