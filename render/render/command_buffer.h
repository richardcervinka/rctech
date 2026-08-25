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

    // Undefined,
    // VertexBufferRead,
    // IndexBufferRead,
    // UniformReadVS,
    // UniformReadFS,
    // StorageReadCS,
    // StorageWriteCS,
    // SampledReadFS,
    // TransferSrc,
    // TransferDst,
    // ColorAttachmentWrite,
    // DepthAttachmentWrite,
    // PresentSrc,
    enum class ImageUsage
    {
        Undefined,
        DepthStencilTest,
        ColorAttachmentWrite,
        Present  // PresentDst
    };

    enum class ImageLayout
    {
        Undefined,
        ColorAttachment,
        DeptAttachment,
        Present
    };

    enum class ImageAccess
    {
        None,
        DepthStencilTest,
        ColorAttachmentWrite
    };

    enum class BufferUsage
    {
        Undefined,
        VertexInput,
        Transfer,
        DescriptorHeap
        // DepthStencilTest,
        // ColorAttachmentWrite,
        // Present  // PresentDst
    };

    enum class BufferAccess
    {
        None,
        VertexAttribute,
        Index,
        TransferSrc,
        TransferDst,
        ResourceDescriptorHeap,
        SamplerDescriptorHeap
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



        //enum class ImageAccess

        //enum class AccessStage 

        void RenderTargetBarier(
            RenderTargetView const& render_target,
            ImageUsage before_usage,
            ImageUsage after_usage,
            ImageLayout before_layout,
            ImageLayout after_layout,
            ImageAccess before_access,
            ImageAccess after_access
        );

        void MemoryBarier(
            BufferRegion const& region,
            BufferUsage before_usage,
            BufferUsage after_usage,
            BufferAccess before_access,
            BufferAccess after_access
        );

        // Set vertex buffer read barrier.
        //void UseVertexBuffer(BufferRegion& region); // TODO
        // Set index buffer read barrier.
        // void UseIndexBuffer(BufferRegion& region); // TODO
        void UseUniformBuffer(BufferRegion& region); // TODO
        
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

        void RenderTargetBarier(
            RenderTargetView const& render_target,
            ImageUsage before_usage,
            ImageUsage after_usage,
            ImageLayout before_layout,
            ImageLayout after_layout,
            ImageAccess before_access,
            ImageAccess after_access
        );

        void MemoryBarier(
            BufferRegion const& region,
            BufferUsage before_usage,
            BufferUsage after_usage,
            BufferAccess before_access,
            BufferAccess after_access
        );

        // New bariers

        void BarierAcquireTextureTransfer(Texture2d const& texture);
        void BarierReleaseTextureTransfer(Texture2d const& texture, uint32_t dst_queue_family_index);

        void TransferBuffer(
            BufferRegion const& src,
            BufferRegion& dst,
            uint32_t dst_queue_family_index = 0
        );

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

        uint32_t vk_queue_family_index {};
    };
    
} // Rc::Render