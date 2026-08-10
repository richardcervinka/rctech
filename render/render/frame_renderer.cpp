#include "frame_renderer.h"
#include <array>

namespace Rc::Render
{
    void Frame::Create(Device const& device, uint32_t width, uint32_t height)
    {
        queue = device.CreateGraphicsQueue();
        fence = device.CreateFence();
        commands = queue->CreateCommandBuffer();
        staging_buffer = std::make_unique<BufferLinearAllocator>(device.AllocateStagingBuffer(staging_buffer_size));
        instance_buffer = device.AllocateInstanceBuffer(2048 * 32); // ---------------------------------------------------------------- Size?
        render_pass_uniform_buffer = device.AllocateUniformBuffer(RenderPassConstants::size);
        depth_buffer = device.AllocateDepthBuffer(width, height);
        depth_buffer_view = depth_buffer->CreateDepthBufferView();

        std::array<ResourceDescriptor, 1> const resource_descriptors
        {
            UniformBufferDescriptor
            {
                .address = render_pass_uniform_buffer->Address(),
                .size = render_pass_uniform_buffer->Size()
            }
        };

        resource_descriptor_heap = device.CreateResourceDescriptorHeap(resource_descriptors);
        resource_descriptor_heap_buffer = device.AllocateDescriptorHeapBuffer(resource_descriptor_heap->SizeTotal());
        resource_descriptor_heap->Attach(*resource_descriptor_heap_buffer);
    }

    void Frame::Resize(Device const& device, uint32_t width, uint32_t height)
    {
        depth_buffer = device.AllocateDepthBuffer(width, height);
        depth_buffer_view = depth_buffer->CreateDepthBufferView();
    }

    void Frame::UpdateResourceDescriptorHeap(Device const& device)
    {
        auto transfer_buffer = device.AllocateStagingBuffer(resource_descriptor_heap->Size());

        const auto staging_region = transfer_buffer->GetRegion(0, resource_descriptor_heap->Size());
        // TODO: Throw when vb_region is nullopt? --------------------------------------------------------
        const auto memory = transfer_buffer->Map(staging_region);

        auto dst_region = resource_descriptor_heap_buffer->GetRegion(0, resource_descriptor_heap->Size());
                
        // Write descriotor heap to the transfer buffer.
        resource_descriptor_heap->Write(memory);

        commands->Reset();
        commands->Begin();

        // Transfer the staging buffer.
        commands->TransferBuffer(staging_region, dst_region);

        // Memory Barrier
        commands->UseResourceDescriptorHeapBuffer(dst_region);

        commands->End();

        // Submit commands.
        fence->Reset();
        queue->Submit(*commands, *fence);
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

        //instance_map = instance_buffer->Map();
        instance_writer = BufferWriter(instance_buffer->Map());
    }

    // void End(SwapChain const& swap_chain)
    // {
    //     commands->UsePresentingFramebuffer(m_swap_chain->GetRenderTargetView());
    //     commands->End();
    // }

    void Frame::BeginTestRenderPass(
        Pipeline const& pipeline,
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

        commands->EnableColorAttachment(RenderTargetSlot::FrameBuffer, framebuffer);
        commands->DisableStencilTest();
        commands->EnableDepthTest();
        commands->EnableDepthWrite();
        commands->SetDepthCompareGreater();
        commands->AttachDepthBuffer(*depth_buffer_view);
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