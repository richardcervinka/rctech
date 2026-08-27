#include "frame_renderer.h"
#include "constants.h"

namespace Rc::Render
{
    void Frame::Resize(Device const& device, uint32_t width, uint32_t height)
    {
        depth_buffer = device.AllocateDepthBuffer(width, height);
        depth_buffer_view = depth_buffer->CreateDepthBufferView();
    }

    void Frame::Begin()
    {
        //std::this_thread::sleep_for(std::chrono::milliseconds(10));
        // std::atomic_signal_fence(std::memory_order_seq_cst);

        fence->Wait();
        fence->Reset(); // ----------------- Do End() ?
        staging_buffer->Reset();
        
        commands->Reset();
        commands->Begin();

        instance_writer = BufferWriter(instance_buffer->Map());
    }

    // void End(SwapChain const& swap_chain)
    // {
    //     commands->UsePresentingFramebuffer(m_swap_chain->GetRenderTargetView());
    //     commands->End();
    // }

    void Frame::BeginTestRenderPass(
        Pipeline const& pipeline,
        ResourceDescriptorHeap const& resource_descriptor_heap,
        SamplerDescriptorHeap const& sampler_descriptor_heap,
        RenderTargetView const& framebuffer,
        RenderPassContext const& context)
    {
        Rectangle<int> const framebuffer_area {0, 0, framebuffer.Width(), framebuffer.Height()};

        // Update uniform buffer
        {
            auto region = render_pass_uniform_buffer->GetRegion(0, RenderPassConstants::size);

            RenderPassConstants constants
            {
                .camera_projection_matrix = context.camera->GetProjectionMatrix(framebuffer.Width(), framebuffer.Height())
            };

            constants.Write(*render_pass_uniform_buffer, region);

            commands->MemoryBarrier(region, BufferUsage::Undefined, BufferUsage::UniformBuffer);  // ------------------------------ NE KAZDY FRAME !!!!!!!!!!!!!!!!
        }

        // Begin rendering

        commands->SetViewport({
            .x = 0,
            .y = 0,
            .width = static_cast<float>(framebuffer_area.w),
            .height = static_cast<float>(framebuffer_area.h),
            .min_depth = 0,
            .max_depth = 1
        });

        commands->SetScissor({
            .x = 0,
            .y = 0,
            .w = framebuffer_area.w,
            .h = framebuffer_area.h
        });

        commands->PushData({
            .ubo_index = uniform_buffer_index
        });

        commands->BindResourceDescriptorHeap(resource_descriptor_heap);
        commands->BindSamplerDescriptorHeap(sampler_descriptor_heap);
        commands->EnableColorAttachment(RenderTargetSlot::FrameBuffer, framebuffer);

        commands->RenderTargetBarrier(
            framebuffer,
            ImageUsage::SwapChainAcquire,
            ImageUsage::ColorAttachment
        );
        
        commands->DisableStencilTest(); // -------------- redundantni? Reset state?
        commands->EnableDepthTest();
        commands->EnableDepthWrite();
        commands->SetDepthCompareGreater();
        commands->AttachDepthBuffer(*depth_buffer_view);
        
        commands->RenderTargetBarrier(
            *depth_buffer_view,
            ImageUsage::Undefined,
            ImageUsage::DepthBuffer
        );

        commands->ClearRenderTarget(RenderTargetSlot::FrameBuffer, Color(0, 0, 0, 1));
        commands->BindPipeline(pipeline);
        commands->BeginRendering(framebuffer_area);
    }

    void Frame::EndRenderPass()
    {
        commands->EndRendering();
    }

    void Frame::BindVertexBuffer(Buffer const& buffer, uint64_t offset)
    {
        commands->BindVertexBuffer(buffer, 0, offset);
    }

    void Frame::BindInstanceBuffer(uint64_t offset)
    {
        commands->BindVertexBuffer(*instance_buffer, 1, offset);
    }

    void Frame::BindIndexBuffer(Buffer const& buffer, IndexType type, uint64_t offset)
    {
        commands->BindIndexBuffer(buffer, type, offset);
    }

    void Frame::Draw(uint32_t index_count, uint32_t instance_count, uint32_t first_index, int32_t vertex_offset, uint32_t first_instance)
    {
        commands->DrawIndexed(index_count, instance_count, first_index, vertex_offset, first_instance);
    }

    void Frame::Wait() const
    {
        fence->Wait();
    }

} // Rc::Render