#pragma once

#include "vulkan/device.h"
#include <memory>
#include <span>
#include <cstddef>

namespace Rc::Render
{
    class Buffer
    {
    public:
        virtual ~Buffer();

        VkBuffer Handle() const { return m_vk_buffer; }

        uint64_t Size() const { return m_vma_allocation_info.size; }

    protected:
        void Create(
            VmaAllocator vma_allocator,
            VmaAllocationCreateInfo const& alloc_info,
            VkBufferCreateInfo const& buffer_info
        );

        friend class CommandBuffer;

        VkBuffer m_vk_buffer {VK_NULL_HANDLE};
        VmaAllocator m_vma_allocator {VK_NULL_HANDLE};
        VmaAllocation m_vma_allocation {nullptr};
        VmaAllocationInfo m_vma_allocation_info {};
        VkAccessFlags m_access_flags {VK_ACCESS_NONE};
        VkPipelineStageFlags m_stage_flags {VK_PIPELINE_STAGE_NONE};
    };

} // Rc::Render