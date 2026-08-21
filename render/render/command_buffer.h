#pragma once

#include "index_buffer.h"
#include "vulkan/device.h"
#include "buffer.h"
#include "base/geometry.h"
#include "base/color.h"
#include <cstdint>
#include "texture.h"
#include "pipeline_state.h"
#include "descriptor_heap.h"

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

    enum class RenderTargetSlot
    {
        FrameBuffer = 0
    };

    struct PushBlock
    {
        uint32_t ubo_index;
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

        void EnableColorAttachment(RenderTargetSlot slot, RenderTargetView const& render_target);

        void DisableColorAttachment(RenderTargetSlot slot);

        void AttachDepthBuffer(RenderTargetView const& render_target);
        void DetachDepthBuffer();

        void ClearRenderTarget(RenderTargetSlot slot, Color const& color);

        void LoadRenderTarget(RenderTargetSlot slot);

        // Swap chain present target barrier
        void UsePresentingFramebuffer(RenderTargetView const& render_target);

        // Set vertex buffer read barrier.
        void UseVertexBuffer(BufferRegion& region);

        // Set index buffer read barrier.
        void UseIndexBuffer(BufferRegion& region);

        void UseUniformBuffer(BufferRegion& region);

        void UseResourceDescriptorHeapBuffer(BufferRegion& region);
        void UseSamplerDescriptorHeapBuffer(BufferRegion& region);

        void TransferBuffer(BufferRegion const& src, BufferRegion& dst);

        //void TransferResourceDescriptorHeap()

        void BindVertexBuffer(Buffer const& vb, int slot, uint64_t offset);

        void BindIndexBuffer(Buffer const& ib, IndexType type, uint64_t offset);

        void BeginRendering(Rectangle<int> const& render_area);
        void EndRendering();

        // No pipeline, no draws — clear done by loadOp
        void BindPipeline(Pipeline const& pipeline);

        void SetViewport(Viewport const& viewport);

        void SetScissor(Rectangle<int> const& rect);

        void PushData(PushBlock const& data) const;

        void Draw(
            uint32_t vertex_count,
            uint32_t instance_count,
            uint32_t first_vertex,
            uint32_t first_instance
        );

        void DrawIndexed(
            uint32_t index_count,
            uint32_t instance_count,
            uint32_t first_index,
            int32_t vertex_offset,
            uint32_t first_instance
        );

        void BindResourceDescriptorHeap(ResourceDescriptorHeap const& heap);
        void BindSamplerDescriptorHeap(SamplerDescriptorHeap const& heap);

        void EnableDepthTest();
        void DisableDepthTest();
        void EnableDepthWrite();
        void DisableDepthWrite();
        void SetDepthCompareGreater();
        void SetDepthCompareLess();
        void EnableStencilTest();
        void DisableStencilTest();
        
        VkCommandBuffer Underlying() const
        {
            return vk_command_buffer;
        }

    private:
        // Parent device.
        VulkanDevice const& vk_device;

        VkCommandPool vk_pool {VK_NULL_HANDLE};

        // Primary command buffers.
        VkCommandBuffer vk_command_buffer {VK_NULL_HANDLE};

        // seconday command buffers...

        //static constexpr int attachments_count = 4;

        // Dynamic rendering color attachments (outputs)
        std::array<VkRenderingAttachmentInfo, 4> color_attachments {};

        std::optional<VkRenderingAttachmentInfo> depth_attachment;
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
        void TransferTexture(BufferRegion const& src, Texture2d& dst);
        
        VkCommandBuffer Underlying() const
        {
            return vk_command_buffer;
        }

    private:
        // Parent device.
        VulkanDevice const& vk_device;

        VkCommandPool vk_pool {VK_NULL_HANDLE};

        // Primary command buffers.
        VkCommandBuffer vk_command_buffer {VK_NULL_HANDLE};
    };
    
} // Rc::Render