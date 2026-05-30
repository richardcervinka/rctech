#include "command_buffer.h"

// ----------test
#include "base/color.h"

namespace Rc
{
    CommandBuffer::CommandBuffer(VulkanDevice const& vk_device, uint32_t vk_family_index) :
        m_vk_device{&vk_device}
    {
        ResetColorAttachments();

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

    CommandBuffer::~CommandBuffer()
    {
        if (m_vk_device != nullptr)
        {
            m_vk_device->FreeCommandBuffer(m_vk_pool, m_vk_command_buffer);
            m_vk_device->DestroyCommandPool(m_vk_pool);
        }
    }

    void CommandBuffer::Reset()
    {
        m_vk_device->ResetCommandPool(m_vk_pool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);

        ResetColorAttachments();
    }

    void CommandBuffer::ResetColorAttachments()
    {
        m_color_attachments_count = 0;

        for (auto& attachment : m_color_attachments)
        {
            attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
            attachment.pNext = nullptr;
            attachment.imageView = VK_NULL_HANDLE;
            attachment.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            attachment.resolveMode = VK_RESOLVE_MODE_NONE;
            attachment.resolveImageView = VK_NULL_HANDLE;
            attachment.resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            attachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            attachment.clearValue = {};
        }
    }

    void CommandBuffer::Begin()
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

    void CommandBuffer::End()
    {
        m_vk_device->EndCommandBuffer(m_vk_command_buffer);
    }

    void CommandBuffer::BarrierRenderFramebuffer(Texture2D const& image)
    {
        VkImageMemoryBarrier const barrier
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .pNext = nullptr,
            .srcAccessMask = VK_ACCESS_NONE,
            .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = image.GetImage(),
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
    }

    void CommandBuffer::BarrierPresentFramebuffer(Texture2D const& image)
    {
        VkImageMemoryBarrier const barrier
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .pNext = nullptr,
            .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .dstAccessMask = VK_ACCESS_NONE,
            .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = image.GetImage(),
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
    }

    void CommandBuffer::SetRenderTargetsCount(int count)
    {
        m_color_attachments_count = count;
    }

    void CommandBuffer::AttachRenderTarget(int slot, TextureView2D const& view)
    {
        // ------------------------------- BarrierRenderFramebuffer()

        auto& dst = m_color_attachments[slot];

        dst.imageView = view.GetView();
        dst.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        dst.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        dst.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    }

    void CommandBuffer::ClearRenderTarget(int slot, Color const& color)
    {
        auto& dst = m_color_attachments[slot];

        dst.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        dst.clearValue.color.float32[0] = color.r;
        dst.clearValue.color.float32[1] = color.g;
        dst.clearValue.color.float32[2] = color.b;
        dst.clearValue.color.float32[3] = color.a;
    }

    void CommandBuffer::SetViewport(Viewport const& viewport)
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

    void CommandBuffer::BeginRendering(Rectangle<int> const& render_area)
    {
        VkRenderingInfo const rendering_info
        {
            .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
            .pNext = nullptr,
            .flags = 0,
            .renderArea = VkRect2D{{0, 0}, {(uint32_t)render_area.w, (uint32_t)render_area.h}},
            .layerCount = 1,
            .viewMask = 0,
            .colorAttachmentCount = static_cast<uint32_t>(m_color_attachments_count),
            .pColorAttachments = m_color_attachments.data(),
            .pDepthAttachment = nullptr,
            .pStencilAttachment = nullptr
        };

        m_vk_device->CmdBeginRendering(m_vk_command_buffer, rendering_info);
    }

    void CommandBuffer::EndRendering()
    {
        m_vk_device->CmdEndRendering(m_vk_command_buffer);
    }

    void CommandBuffer::BindPipeline(Pipeline const& pipeline)
    {
        m_vk_device->CmdBindPipeline(m_vk_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.Handle());
    }

    void CommandBuffer::TransferBuffer(StagingBuffer& src, Buffer& dst, uint64_t src_offset, uint64_t dst_offset, uint64_t size)
    {
        VkBufferMemoryBarrier const barrier
        {
            .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
            .pNext = nullptr,
            .srcAccessMask = dst.m_access_flags,
            .dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .buffer = dst.Handle(),
            .offset = VkDeviceSize{dst_offset},
            .size = VkDeviceSize{size}
        };

        m_vk_device->CmdPipelineBarrier(
            m_vk_command_buffer,
            dst.m_stage_flags,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            {},
            {},
            {&barrier, 1},
            {}
        );

        dst.m_access_flags = VK_ACCESS_TRANSFER_WRITE_BIT;
        dst.m_stage_flags = VK_PIPELINE_STAGE_TRANSFER_BIT;

        struct VkBufferCopy region
        {
            .srcOffset = VkDeviceSize{src_offset},
            .dstOffset = VkDeviceSize{dst_offset},
            .size = VkDeviceSize{size}
        };
        
        m_vk_device->CmdCopyBuffer(m_vk_command_buffer, src.Handle(), dst.Handle(), {&region, 1});
    }

    void CommandBuffer::UseBuffer(VertexBuffer& vb, uint64_t offset, uint64_t size)
    {
        VkBufferMemoryBarrier const barrier
        {
            .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
            .pNext = nullptr,
            .srcAccessMask = vb.m_access_flags,
            .dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .buffer = vb.Handle(),
            .offset = static_cast<VkDeviceSize>(offset),
            .size = static_cast<VkDeviceSize>(size)
        };

        m_vk_device->CmdPipelineBarrier(
            m_vk_command_buffer,
            vb.m_stage_flags,
            VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
            {},
            {},
            {&barrier, 1},
            {}
        );

        vb.m_access_flags = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
        vb.m_stage_flags = VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
    }

    void CommandBuffer::UseBuffer(IndexBuffer& ib, uint64_t offset, uint64_t size)
    {
        VkBufferMemoryBarrier const barrier
        {
            .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
            .pNext = nullptr,
            .srcAccessMask = ib.m_access_flags,
            .dstAccessMask = VK_ACCESS_INDEX_READ_BIT,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .buffer = ib.Handle(),
            .offset = static_cast<VkDeviceSize>(offset),
            .size = static_cast<VkDeviceSize>(size)
        };

        m_vk_device->CmdPipelineBarrier(
            m_vk_command_buffer,
            ib.m_stage_flags,
            VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
            {},
            {},
            {&barrier, 1},
            {}
        );

        ib.m_access_flags = VK_ACCESS_INDEX_READ_BIT;
        ib.m_stage_flags = VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
    }

    void CommandBuffer::BindVertexBuffer(VertexBuffer& vb, uint64_t offset)
    {
        const std::array<VkDeviceSize, 1> vk_offset {offset};
        const std::array<VkBuffer, 1> vk_buffers {vb.Handle()};

        m_vk_device->CmdBindVertexBuffers(m_vk_command_buffer, 0, 1, vk_buffers, vk_offset);
    }

    void CommandBuffer::BindIndexBuffer16(IndexBuffer& ib, uint64_t offset)
    {
        m_vk_device->CmdBindIndexBuffer(m_vk_command_buffer, ib.Handle(), offset, VK_INDEX_TYPE_UINT16);
    }

    void CommandBuffer::BindIndexBuffer32(IndexBuffer& ib, uint64_t offset)
    {
        m_vk_device->CmdBindIndexBuffer(m_vk_command_buffer, ib.Handle(), offset, VK_INDEX_TYPE_UINT32);
    }

    void CommandBuffer::Draw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance)
    {
        m_vk_device->CmdDraw(m_vk_command_buffer, vertex_count, instance_count, first_vertex, first_instance);
    }

    void CommandBuffer::DrawIndexed(uint32_t index_count, uint32_t instance_count, uint32_t first_index, int32_t vertex_offset, uint32_t first_instance)
    {
        m_vk_device->CmdDrawIndexed(m_vk_command_buffer, index_count, instance_count, first_index, vertex_offset, first_instance);
    }

    void CommandBuffer::Test(Rectangle<int> const& render_area)
    {
        VkRect2D const scissors
        {
            {0, 0}, {(uint32_t)render_area.w, (uint32_t)render_area.h}
        };

        SetViewport({0, 0, static_cast<float>(render_area.w), static_cast<float>(render_area.h), 0, 1});

        m_vk_device->CmdSetScissor(m_vk_command_buffer, scissors);
    }

} // Rc