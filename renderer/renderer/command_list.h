#pragma once

#include "vulkan/device.h"
#include "buffer.h"
#include "base/geometry.h"
#include "base/color.h"
#include <array>
#include "image.h"
#include "texture.h"
#include "swap_chain.h" //----------------------- framebuffer
#include "staging_buffer.h"

namespace Rc
{
    class CommandBuffer
    {
    public:
        CommandBuffer(VulkanDevice const& vk_device, uint32_t vk_family_index);

        ~CommandBuffer();

        CommandBuffer(CommandBuffer const&) = delete;
        CommandBuffer& operator=(CommandBuffer const&) = delete;
        CommandBuffer(CommandBuffer&& other) = delete;
        CommandBuffer& operator=(CommandBuffer&& other) = delete;

        void Reset();

        // Bedin base render commands.
        void Begin();

        void End();

        // Swap chain render target barrier
        void BarrierRenderFramebuffer(Texture2D const& image);

        // Swap chain present target barrier
        void BarrierPresentFramebuffer(Texture2D const& image);

        void TransferBuffer(StagingBuffer& src, int offset, int size);

        // void BeginRenderPass()

        VkCommandBuffer Buffer() { return m_vk_buffer; } //--------------------- delete?

        void SetRenderTargetsCount(int count);
        void AttachRenderTarget(int slot, TextureView2D const& view);

        void ClearRenderTarget(int slot, Color const& color);
        // TODO: KeepRenderTarget ro disable clearing...

        //void Transfer(Buffer const& src, Buffer const& dst);

        void Test(
            Rectangle<int> const& render_area,
            VkPipeline const& pipeline
        );

    private:
        friend class CommandQueue;

        void ResetColorAttachments();

        // Parent device.
        VulkanDevice const* m_vk_device {nullptr};

        VkCommandPool m_vk_pool {VK_NULL_HANDLE};

        // Primary command buffers.
        VkCommandBuffer m_vk_buffer {VK_NULL_HANDLE}; // --------------------- rename to vk_command_buffer

        int m_color_attachments_count {0};

        // Dynamic rendering color attachments (outputs)
        std::array<VkRenderingAttachmentInfo, 4> m_color_attachments {};

        // seconday command buffers...
    };

} // Rc