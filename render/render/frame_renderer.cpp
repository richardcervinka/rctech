#include "frame_renderer.h"
#include <array>

namespace Rc::Render
{
    void Frame::Create(Device const& device)
    {
        m_queue = device.CreateGraphicsQueue();
        fence = device.CreateFence();
        commands = m_queue->CreateCommandBuffer();
        staging_buffer = std::make_unique<BufferLinearAllocator>(device.AllocateBuffer(StagingBufferInfo{.size = staging_buffer_size}));
        uniform_buffer = device.AllocateBuffer(UniformBufferInfo{.size = uniform_buffer_size});

        std::array<ResourceDescriptor, 1> const resource_descriptors
        {
            UniformBufferDescriptor
            {
                .address = uniform_buffer->Address(),
                .size = uniform_buffer->Size()
            }
        };

        resource_descriptor_heap = device.CreateResourceDescriptorHeap(resource_descriptors);
        resource_descriptor_heap_buffer = device.AllocateBuffer(DescriptorHeapBufferInfo{.size = resource_descriptor_heap->SizeTotal()});
        resource_descriptor_heap->Attach(*resource_descriptor_heap_buffer);
    }

    void Frame::UpdateResourceDescriptorHeapBuffer(Device const& device)
    {
        auto transfer_buffer = device.AllocateBuffer(StagingBufferInfo{.size = resource_descriptor_heap->Size()});

        const auto region = transfer_buffer->GetRegion(0, resource_descriptor_heap->Size());
        // TODO: Throw when vb_region is nullopt? --------------------------------------------------------
        const auto memory = transfer_buffer->Map(region);
                
        // Write descriotor heap to the transfer buffer.
        resource_descriptor_heap->Write(memory);

        commands->Reset();
        commands->Begin();

        // Transfer the staging buffer.
        commands->TransferBuffer(
            *transfer_buffer,
            *resource_descriptor_heap_buffer,
            region.Offset(),
            0,
            region.Size()
        );

        // Memory Barrier
        commands->UseResourceDescriptorHeapBuffer(
            *resource_descriptor_heap_buffer,
            resource_descriptor_heap_buffer->GetRegion()
        );

        // Submit commands.
        commands->End();
        fence->Reset();
        m_queue->Submit(*commands, *fence);
        fence->Wait();
    }

    void Frame::Begin()
    {
        fence->Wait();
        fence->Reset();
        staging_buffer->Reset();
        
        commands->Reset();
        commands->Begin();
        commands->BindResourceDescriptorHeap(*resource_descriptor_heap);
    }

    // void End(SwapChain const& swap_chain)
    // {
    //     commands->UsePresentingFramebuffer(m_swap_chain->GetRenderTargetView());
    //     commands->End();
    // }

    void Frame::BeginTestRenderPass(
        Pipeline const& pipeline,
        RenderTargetView const& framebuffer)
    {
        // Framebuffer memory barrier.
        commands->UseRenderingFramebuffer(framebuffer);

        RenderTargetAttachments attachments;
        attachments.EnableColorAttachment(RenderTargetSlot::FrameBuffer, framebuffer);
        attachments.ClearRenderTarget(RenderTargetSlot::FrameBuffer, Color(0, 0, 0, 1));

        Rectangle<int> const framebuffer_area {0, 0, framebuffer.Width(), framebuffer.Height()};

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

        commands->BindPipeline(pipeline);
        commands->BeginRendering(framebuffer_area, attachments);
    }

    void Frame::EndRenderPass()
    {
        commands->EndRendering();
    }

    void Frame::Wait() const
    {
        fence->Wait();
    }

} // Rc::Render