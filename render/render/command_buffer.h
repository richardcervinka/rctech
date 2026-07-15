#pragma once

#include "index_buffer.h"
#include "vulkan/device.h"
#include "buffer.h"
#include "base/geometry.h"
#include <cstdint>
#include "texture.h"
#include "pipeline_state.h"
#include "descriptor_heap.h"
#include "attachment.h"

namespace Rc::Render
{
    struct Viewport
    {
        float x;
        float y;
        float width;
        float height;
        float min_depth;
        float max_depth;
    };

    class RenderCommandBuffer
    {
    public:
        RenderCommandBuffer(VulkanDevice const& vk_device, uint32_t vk_family_index);

        ~RenderCommandBuffer();

        RenderCommandBuffer(RenderCommandBuffer const&) = delete;
        RenderCommandBuffer& operator=(RenderCommandBuffer const&) = delete;
        RenderCommandBuffer(RenderCommandBuffer&& other) = delete;
        RenderCommandBuffer& operator=(RenderCommandBuffer&& other) = delete;

        void Reset();

        // Bedin base render commands.
        void Begin();

        void End();

        // Swap chain render target barrier
        void UseRenderingFramebuffer(RenderTargetView const& render_target);

        // Swap chain present target barrier
        void UsePresentingFramebuffer(RenderTargetView const& render_target);

        // Set vertex buffer read barrier.
        void UseVertexBuffer(BufferRegion& region);

        // Set index buffer read barrier.
        void UseIndexBuffer(BufferRegion& region);

        void UseUniformBuffer(BufferRegion& region);

        void UseResourceDescriptorHeapBuffer(BufferRegion& region);

        //void TransferBuffer(Buffer& src, Buffer& dst, uint64_t src_offset, uint64_t dst_offset, uint64_t size);
        void TransferBuffer(BufferRegion const& src, BufferRegion& dst);

        void BindVertexBuffer(Buffer const& vb, int slot, uint64_t offset);

        void BindIndexBuffer(Buffer const& ib, IndexType type, uint64_t offset);

        void BeginRendering(Rectangle<int> const& render_area, RenderTargetAttachments const& attachments);
        void EndRendering();

        // No pipeline, no draws — clear done by loadOp
        void BindPipeline(Pipeline const& pipeline);

        void SetViewport(Viewport const& viewport);

        void SetScissor(Rectangle<int> const& rect);

        void Draw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance);

        void DrawIndexed(uint32_t index_count, uint32_t instance_count, uint32_t first_index, int32_t vertex_offset, uint32_t first_instance);

        void BindResourceDescriptorHeap(ResourceDescriptorHeap const& heap);

        VkCommandBuffer Handle() const
        {
            return vk_command_buffer;
        }

    private:
        // Parent device.
        VulkanDevice const* vk_device {nullptr};

        VkCommandPool vk_pool {VK_NULL_HANDLE};

        // Primary command buffers.
        VkCommandBuffer vk_command_buffer {VK_NULL_HANDLE};

        // seconday command buffers...
    };

    class TransferCommandBuffer
    {
    public:
        TransferCommandBuffer(VulkanDevice const& vk_device, uint32_t vk_family_index);

        ~TransferCommandBuffer();

        TransferCommandBuffer(TransferCommandBuffer const&) = delete;
        TransferCommandBuffer& operator=(TransferCommandBuffer const&) = delete;
        TransferCommandBuffer(TransferCommandBuffer&& other) = delete;
        TransferCommandBuffer& operator=(TransferCommandBuffer&& other) = delete;

        void Reset();

        // Bedin base render commands.
        void Begin();

        void End();

        void TransferBuffer(BufferRegion const& src, BufferRegion& dst);
        
        VkCommandBuffer Handle() const
        {
            return vk_command_buffer;
        }

    private:
        // Parent device.
        VulkanDevice const* vk_device {nullptr};

        VkCommandPool vk_pool {VK_NULL_HANDLE};

        // Primary command buffers.
        VkCommandBuffer vk_command_buffer {VK_NULL_HANDLE};
    };
    
} // Rc::Render