#include "buffer.h"
#include <stdexcept>
#include <cassert>

namespace Rc::Render
{
    Buffer::~Buffer()
    {
        if (m_vma_allocator != VK_NULL_HANDLE)
        {
            vmaDestroyBuffer(m_vma_allocator, m_vk_buffer, m_vma_allocation);
        }
    }

    Buffer::Buffer(VulkanDevice const& vk_device, VertexBufferInfo const& info, VmaAllocator vma_allocator) :
        Buffer(
            vk_device,
            info.size,
            vma_allocator,
            VkBufferUsageFlags
            {
                VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
            },
            VmaAllocationCreateFlags{}
        )
    {}

    Buffer::Buffer(VulkanDevice const& vk_device, IndexBufferInfo const& info, VmaAllocator vma_allocator) :
        Buffer(
            vk_device,
            info.size,
            vma_allocator,
            VkBufferUsageFlags
            {
                VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                VK_BUFFER_USAGE_INDEX_BUFFER_BIT
            },
            VmaAllocationCreateFlags{}
        )
    {}

    Buffer::Buffer(VulkanDevice const& vk_device, StagingBufferInfo const& info, VmaAllocator vma_allocator) :
        Buffer(
            vk_device,
            info.size,
            vma_allocator,
            VkBufferUsageFlags
            {
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT
            },
            VmaAllocationCreateFlags
            {
                VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                VMA_ALLOCATION_CREATE_MAPPED_BIT
            }
        )
    {}

    Buffer::Buffer(VulkanDevice const& vk_device, UniformBufferInfo const& info, VmaAllocator vma_allocator) :
        Buffer(
            vk_device,
            info.size,
            vma_allocator,
            VkBufferUsageFlags
            {
                VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT |
                VK_BUFFER_USAGE_2_SHADER_DEVICE_ADDRESS_BIT
            },
            VmaAllocationCreateFlags{}
        )
    {}

    Buffer::Buffer(VulkanDevice const& vk_device, DescriptorHeapBufferInfo const& info, VmaAllocator vma_allocator) :
        Buffer(
            vk_device,
            info.size,
            vma_allocator,
            VkBufferUsageFlags
            {
                VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                VK_BUFFER_USAGE_DESCRIPTOR_HEAP_BIT_EXT |
                VK_BUFFER_USAGE_2_SHADER_DEVICE_ADDRESS_BIT
            },
            VmaAllocationCreateFlags{}
        )
    {}

    Buffer::Buffer(
        VulkanDevice const& vk_device,
        uint64_t size,
        VmaAllocator vma_allocator,
        VkBufferUsageFlags usage,
        VmaAllocationCreateFlags vma_flags) :
            m_vk_device{&vk_device}
    {
        VmaAllocationCreateInfo alloc_info {};
        alloc_info.usage = VMA_MEMORY_USAGE_AUTO;
        alloc_info.flags = vma_flags;

        VkBufferCreateInfo buffer_info
        {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .size = size,
            .usage =  usage,
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
            throw VulkanException(vk_result);
        }

        m_vma_allocator = vma_allocator;   
    }

    BufferRegion Buffer::GetRegion() const
    {
        return {0, Size()};
    }

    BufferRegion Buffer::GetRegion(uint64_t offset, uint64_t size) const
    {
        if (offset + size > Size())
        {
            throw std::runtime_error("REFACTOR ME"); //--------------------------
        }

        return {offset, size};
    }

    std::span<std::byte> Buffer::Map(uint64_t offset, uint64_t size)
    {
        assert(offset + size <= m_vma_allocation_info.size);

        return {
            static_cast<std::byte*>(m_vma_allocation_info.pMappedData) + offset,
            static_cast<std::size_t>(size)
        };
    }

    std::span<std::byte const> Buffer::Map(uint64_t offset, uint64_t size) const
    {
        assert(offset + size <= m_vma_allocation_info.size);

        return {
            static_cast<std::byte const*>(m_vma_allocation_info.pMappedData) + offset,
            static_cast<std::size_t>(size)
        };
    }

    VkDeviceAddress Buffer::Address() const
    {
        VkBufferDeviceAddressInfo const info
        {
            .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
            .pNext = nullptr,
            .buffer = m_vk_buffer
        };

        return m_vk_device->GetBufferDeviceAddress(info);
    }

} // Rc::Render