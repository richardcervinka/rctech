#include "command_buffer.h"

// ----------test
#include "base/color.h"
#include <utility>

namespace Rc::Render
{
    // RenderCommandBuffer
    
    RenderCommandBuffer::RenderCommandBuffer(VulkanDevice const& vk_device, uint32_t vk_family_index) :
        m_vk_device{&vk_device}
    {
        VkCommandPoolCreateInfo const command_pool_info
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueFamilyIndex = vk_family_index
        };

        m_vk_pool = m_vk_device->CreateCommandPool(command_pool_info);

        VkCommandBufferAllocateInfo const allocate_info
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = nullptr,
            .commandPool = m_vk_pool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1
        };

        m_vk_command_buffer = m_vk_device->AllocateCommandBuffer(allocate_info);
    }

    RenderCommandBuffer::~RenderCommandBuffer()
    {
        if (m_vk_device != nullptr)
        {
            m_vk_device->FreeCommandBuffer(m_vk_pool, m_vk_command_buffer);
            m_vk_device->DestroyCommandPool(m_vk_pool);
        }
    }

    void RenderCommandBuffer::Reset()
    {
        m_vk_device->ResetCommandPool(m_vk_pool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);
    }

    void RenderCommandBuffer::Begin()
    {
        VkCommandBufferBeginInfo const begin_info
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = 0,
            .pInheritanceInfo = nullptr
        };

        m_vk_device->BeginCommandBuffer(m_vk_command_buffer, begin_info);

        // Use VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT  -----------------------?
    }

    void RenderCommandBuffer::End()
    {
        m_vk_device->EndCommandBuffer(m_vk_command_buffer);
    }

    void RenderCommandBuffer::UseRenderingFramebuffer(RenderTargetView const& render_target)
    {
        VkImageMemoryBarrier const barrier
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .pNext = nullptr,
            .srcAccessMask = VK_ACCESS_NONE,
            .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .oldLayout = render_target.m_layout,
            .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = render_target.Image(),
            .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}
        };

        m_vk_device->CmdPipelineBarrier(
            m_vk_command_buffer,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            {},
            {},
            {},
            {&barrier, 1}
        );

        render_target.m_layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }

    void RenderCommandBuffer::UsePresentingFramebuffer(RenderTargetView const& render_target)
    {
        VkImageMemoryBarrier const barrier
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .pNext = nullptr,
            .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .dstAccessMask = VK_ACCESS_NONE,
            .oldLayout = render_target.m_layout,
            .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = render_target.Image(),
            .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}
        };

        m_vk_device->CmdPipelineBarrier(
            m_vk_command_buffer,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            {},
            {},
            {},
            {&barrier, 1}
        );

        render_target.m_layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    }

    void RenderCommandBuffer::SetViewport(Viewport const& viewport)
    {
        VkViewport const vp {
            .x = viewport.x,
            .y = viewport.y,
            .width = viewport.width,
            .height = viewport.height,
            .minDepth = viewport.min_depth,
            .maxDepth = viewport.max_depth
        };

        m_vk_device->CmdSetViewport(m_vk_command_buffer, vp);  
    }

    void RenderCommandBuffer::BeginRendering(Rectangle<int> const& render_area, RenderTargetAttachments const& attachments)
    {
        auto const& color_attachments = attachments.ColorAttachmentsInfo();

        VkRenderingInfo const rendering_info
        {
            .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
            .pNext = nullptr,
            .flags = 0,
            .renderArea = {{0, 0}, {static_cast<uint32_t>(render_area.w), static_cast<uint32_t>(render_area.h)}},
            .layerCount = 1,
            .viewMask = 0,
            .colorAttachmentCount = static_cast<uint32_t>(color_attachments.size()),
            .pColorAttachments = color_attachments.data(),
            .pDepthAttachment = nullptr,
            .pStencilAttachment = nullptr
        };

        m_vk_device->CmdBeginRendering(m_vk_command_buffer, rendering_info);
    }

    void RenderCommandBuffer::EndRendering()
    {
        m_vk_device->CmdEndRendering(m_vk_command_buffer);
    }

    void RenderCommandBuffer::BindPipeline(Pipeline const& pipeline)
    {
        m_vk_device->CmdBindPipeline(m_vk_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.Handle());
    }

    void RenderCommandBuffer::TransferBuffer(Buffer& src, Buffer& dst, uint64_t src_offset, uint64_t dst_offset, uint64_t size)
    {
        VkBufferMemoryBarrier2 const barrier
        {
            .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
            .pNext = nullptr,
            .srcStageMask = src.m_stage_flags,
            .srcAccessMask = src.m_access_flags,
            .dstStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
            .dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .buffer = dst.Handle(),
            .offset = VkDeviceSize{dst_offset},
            .size = VkDeviceSize{size}
        };

        VkDependencyInfo const dependency_info
        {
            .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .pNext = nullptr,
            .dependencyFlags = {},
            .memoryBarrierCount = 0,
            .pMemoryBarriers = nullptr,
            .bufferMemoryBarrierCount = 1,
            .pBufferMemoryBarriers = &barrier,
            .imageMemoryBarrierCount = 0,
            .pImageMemoryBarriers = nullptr
        };

        dst.m_access_flags = VK_ACCESS_2_TRANSFER_WRITE_BIT;
        dst.m_stage_flags = VK_PIPELINE_STAGE_2_TRANSFER_BIT;

        struct VkBufferCopy region
        {
            .srcOffset = VkDeviceSize{src_offset},
            .dstOffset = VkDeviceSize{dst_offset},
            .size = VkDeviceSize{size}
        };
        
        m_vk_device->CmdCopyBuffer(m_vk_command_buffer, src.Handle(), dst.Handle(), {&region, 1});
    }

    void RenderCommandBuffer::UseVertexBuffer(Buffer& buffer, BufferRegion const& region)
    {
        // TODO: Assert Buffer::usage

        VkBufferMemoryBarrier2 const barrier
        {
            .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
            .pNext = nullptr,
            .srcStageMask = buffer.m_stage_flags,
            .srcAccessMask = buffer.m_access_flags,
            .dstStageMask = VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT,
            .dstAccessMask = VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .buffer = buffer.Handle(),
            .offset = region.Offset(),
            .size = region.Size()
        };

        VkDependencyInfo const dependency_info
        {
            .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .pNext = nullptr,
            .dependencyFlags = {},
            .memoryBarrierCount = 0,
            .pMemoryBarriers = nullptr,
            .bufferMemoryBarrierCount = 1,
            .pBufferMemoryBarriers = &barrier,
            .imageMemoryBarrierCount = 0,
            .pImageMemoryBarriers = nullptr
        };

        buffer.m_access_flags = VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT;
        buffer.m_stage_flags = VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT;
    }

    void RenderCommandBuffer::UseIndexBuffer(Buffer& buffer, BufferRegion const& region)
    {
        // TODO: Assert Buffer::usage

        VkBufferMemoryBarrier2 const barrier
        {
            .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
            .pNext = nullptr,
            .srcStageMask = buffer.m_stage_flags,
            .srcAccessMask = buffer.m_access_flags,
            .dstStageMask = VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT,
            .dstAccessMask = VK_ACCESS_2_INDEX_READ_BIT,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .buffer = buffer.Handle(),
            .offset = region.Offset(),
            .size = region.Size()
        };

        VkDependencyInfo const dependency_info
        {
            .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .pNext = nullptr,
            .dependencyFlags = {},
            .memoryBarrierCount = 0,
            .pMemoryBarriers = nullptr,
            .bufferMemoryBarrierCount = 1,
            .pBufferMemoryBarriers = &barrier,
            .imageMemoryBarrierCount = 0,
            .pImageMemoryBarriers = nullptr
        };

        m_vk_device->CmdPipelineBarrier2(m_vk_command_buffer, dependency_info);

        buffer.m_access_flags = VK_ACCESS_2_INDEX_READ_BIT;
        buffer.m_stage_flags = VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT;
    }

    void RenderCommandBuffer::UseUniformBuffer(Buffer& buffer, BufferRegion const& region)
    {
        // TODO: Assert Buffer::usage

        VkBufferMemoryBarrier2 const barrier
        {
            .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
            .pNext = nullptr,
            .srcStageMask = buffer.m_stage_flags,
            .srcAccessMask = buffer.m_access_flags,
            .dstStageMask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT,
            .dstAccessMask = VK_ACCESS_2_UNIFORM_READ_BIT,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .buffer = buffer.Handle(),
            .offset = region.Offset(),
            .size = region.Size()
        };

        VkDependencyInfo const dependency_info
        {
            .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .pNext = nullptr,
            .dependencyFlags = {},
            .memoryBarrierCount = 0,
            .pMemoryBarriers = nullptr,
            .bufferMemoryBarrierCount = 1,
            .pBufferMemoryBarriers = &barrier,
            .imageMemoryBarrierCount = 0,
            .pImageMemoryBarriers = nullptr
        };

        m_vk_device->CmdPipelineBarrier2(m_vk_command_buffer, dependency_info);

        buffer.m_access_flags = VK_ACCESS_2_UNIFORM_READ_BIT;
        buffer.m_stage_flags = VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT;
    }

    void RenderCommandBuffer::UseResourceDescriptorHeapBuffer(Buffer& buffer, BufferRegion const& region)
    {
        // TODO: Assert Buffer::usage

        VkBufferMemoryBarrier2 const barrier
        {
            .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
            .pNext = nullptr,
            .srcStageMask = buffer.m_stage_flags,
            .srcAccessMask = buffer.m_access_flags,
            .dstStageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
            .dstAccessMask = VK_ACCESS_2_RESOURCE_HEAP_READ_BIT_EXT,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .buffer = buffer.Handle(),
            .offset = region.Offset(),
            .size = region.Size()
        };

        VkDependencyInfo const dependency_info
        {
            .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .pNext = nullptr,
            .dependencyFlags = {},
            .memoryBarrierCount = 0,
            .pMemoryBarriers = nullptr,
            .bufferMemoryBarrierCount = 1,
            .pBufferMemoryBarriers = &barrier,
            .imageMemoryBarrierCount = 0,
            .pImageMemoryBarriers = nullptr
        };

        m_vk_device->CmdPipelineBarrier2(m_vk_command_buffer, dependency_info);

        buffer.m_access_flags = VK_ACCESS_2_RESOURCE_HEAP_READ_BIT_EXT;
        buffer.m_stage_flags = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT;
    }

    void RenderCommandBuffer::BindVertexBuffer(Buffer& vb, uint64_t offset)
    {
        // TODO: Assert Buffer::usage
        const std::array<VkDeviceSize, 1> vk_offset {offset};
        const std::array<VkBuffer, 1> vk_buffers {vb.Handle()};

        m_vk_device->CmdBindVertexBuffers(m_vk_command_buffer, 0, 1, vk_buffers, vk_offset);
    }

    void RenderCommandBuffer::BindIndexBuffer(Buffer& ib, IndexType type, uint64_t offset)
    {
        // TODO: Assert Buffer::usage
        
        switch (type)
        {
            case IndexType::Uint16:
                m_vk_device->CmdBindIndexBuffer(m_vk_command_buffer, ib.Handle(), offset, VK_INDEX_TYPE_UINT16);
                return;

            case IndexType::Uint32:
                m_vk_device->CmdBindIndexBuffer(m_vk_command_buffer, ib.Handle(), offset, VK_INDEX_TYPE_UINT32);
                return;

            default:
                std::unreachable();
        }
    }

    void RenderCommandBuffer::Draw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance)
    {
        m_vk_device->CmdDraw(m_vk_command_buffer, vertex_count, instance_count, first_vertex, first_instance);
    }

    void RenderCommandBuffer::DrawIndexed(uint32_t index_count, uint32_t instance_count, uint32_t first_index, int32_t vertex_offset, uint32_t first_instance)
    {
        m_vk_device->CmdDrawIndexed(m_vk_command_buffer, index_count, instance_count, first_index, vertex_offset, first_instance);
    }

    void RenderCommandBuffer::Test(Rectangle<int> const& render_area)
    {
        VkRect2D const scissors
        {
            {0, 0}, {(uint32_t)render_area.w, (uint32_t)render_area.h}
        };

        SetViewport({0, 0, static_cast<float>(render_area.w), static_cast<float>(render_area.h), 0, 1});

        m_vk_device->CmdSetScissor(m_vk_command_buffer, scissors);
    }

    void RenderCommandBuffer::BindResourceDescriptorHeap(ResourceDescriptorHeap const& heap)
    {
        VkBindHeapInfoEXT bind_info
        {
            .sType = VK_STRUCTURE_TYPE_BIND_HEAP_INFO_EXT,
            .pNext = nullptr,
            .heapRange.address = heap.Address(),
            .heapRange.size = heap.SizeTotal(),
            .reservedRangeOffset = heap.Size(),
            .reservedRangeSize = heap.Reserved()
        };

        m_vk_device->CmdBindResourceHeapEXT(m_vk_command_buffer, bind_info);
    }

    // TransferCommandBuffer

    TransferCommandBuffer::TransferCommandBuffer(VulkanDevice const& vk_device, uint32_t vk_family_index) :
        m_vk_device{&vk_device}
    {
        VkCommandPoolCreateInfo const command_pool_info
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueFamilyIndex = vk_family_index
        };

        m_vk_pool = m_vk_device->CreateCommandPool(command_pool_info);

        VkCommandBufferAllocateInfo const allocate_info
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = nullptr,
            .commandPool = m_vk_pool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1
        };

        m_vk_command_buffer = m_vk_device->AllocateCommandBuffer(allocate_info);
    }

    TransferCommandBuffer::~TransferCommandBuffer()
    {
        if (m_vk_device != nullptr)
        {
            m_vk_device->FreeCommandBuffer(m_vk_pool, m_vk_command_buffer);
            m_vk_device->DestroyCommandPool(m_vk_pool);
        }
    }

    void TransferCommandBuffer::Reset()
    {
        m_vk_device->ResetCommandPool(m_vk_pool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);
    }


    void TransferCommandBuffer::Begin()
    {
        VkCommandBufferBeginInfo const begin_info
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = 0,
            .pInheritanceInfo = nullptr
        };

        m_vk_device->BeginCommandBuffer(m_vk_command_buffer, begin_info);

        // Use VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT  -----------------------?
    }

    void TransferCommandBuffer::End()
    {
        m_vk_device->EndCommandBuffer(m_vk_command_buffer);
    }

    void TransferCommandBuffer::TransferBuffer(Buffer const& src, Buffer& dst, uint64_t src_offset, uint64_t dst_offset, uint64_t size)
    {
        VkBufferMemoryBarrier2 const barrier
        {
            .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
            .pNext = nullptr,
            .srcStageMask = src.m_stage_flags,
            .srcAccessMask = src.m_access_flags,
            .dstStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
            .dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .buffer = dst.Handle(),
            .offset = VkDeviceSize{dst_offset},
            .size = VkDeviceSize{size}
        };

        VkDependencyInfo const dependency_info
        {
            .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .pNext = nullptr,
            .dependencyFlags = {},
            .memoryBarrierCount = 0,
            .pMemoryBarriers = nullptr,
            .bufferMemoryBarrierCount = 1,
            .pBufferMemoryBarriers = &barrier,
            .imageMemoryBarrierCount = 0,
            .pImageMemoryBarriers = nullptr
        };

        dst.m_access_flags = VK_ACCESS_2_TRANSFER_WRITE_BIT;
        dst.m_stage_flags = VK_PIPELINE_STAGE_2_TRANSFER_BIT;

        struct VkBufferCopy region
        {
            .srcOffset = VkDeviceSize{src_offset},
            .dstOffset = VkDeviceSize{dst_offset},
            .size = VkDeviceSize{size}
        };
        
        m_vk_device->CmdCopyBuffer(m_vk_command_buffer, src.Handle(), dst.Handle(), {&region, 1});
    }

} // Rc::Render