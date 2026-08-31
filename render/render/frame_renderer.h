#pragma once

#include "texture.h"
#include "buffer_linear_allocator.h"
#include "buffer_writer.h"
#include "descriptor_heap.h"
#include "pipeline_state.h"
#include "fence.h"
#include "command_buffer.h"
#include "device.h"
#include "core/camera.h"
#include "core/vertex.h"

namespace Rc::Render
{
    struct FrameData
    {
        //
    };

    // struct RenderPassVertexBinding
    // {
    //     Buffer const* buffer;
    //     uint64_t offset;

    // };

    struct RenderPassContext
    {
        Gfx::Camera const* camera;
    };

    class Frame
    {
    public:
        //static constexpr uint64_t staging_buffer_size = 2048;
        //static constexpr uint64_t render_pass_uniform_buffer_size = 16 * 4;

        void Resize(Device const& device, uint32_t width, uint32_t height); // ---------------- Remove ?

        void Begin(RenderTargetView const& framebuffer);

        void End(RenderTargetView const& framebuffer);

        void BeginTestRenderPass(
            Pipeline const& pipeline,
            RenderTargetView const& framebuffer,
            RenderPassContext const& context
        );

        void BindResourceDescriptorHeap(ResourceDescriptorHeap const& heap);
        void BindSamplerDescriptorHeap(SamplerDescriptorHeap const& heap);

        void BindVertexBuffer(Buffer const& buffer, uint64_t offset);
        void BindInstanceBuffer(uint64_t offset);
        void BindIndexBuffer(Buffer const& buffer, IndexType type, uint64_t offset);

        void Draw(uint32_t index_count, uint32_t instance_count, uint32_t first_index, int32_t vertex_offset, uint32_t first_instance);

        void EndRenderPass();

        void WriteInstance(Gfx::VertexInstance const& data)
        {
            instance_writer << data.local_transformations;
            instance_writer << data.world_transformations;
        }

        // void Draw(
        //     uint32_t vertex_input_binding,
        //     uint32_t instance_input_binding,
        // );

        void Wait() const;

        uint32_t uniform_buffer_index {};

        //std::unique_ptr<RenderCommandQueue> queue;

        std::unique_ptr<Fence> fence;
        
        std::unique_ptr<RenderCommandBuffer> commands;

        std::unique_ptr<BufferLinearAllocator> staging_buffer;

        std::unique_ptr<Buffer> instance_buffer;

        std::span<std::byte> instance_map;

        BufferWriter instance_writer; //--------------------

        std::unique_ptr<Buffer> render_pass_uniform_buffer;

        std::unique_ptr<Texture2d> depth_buffer;
        std::unique_ptr<RenderTargetView> depth_buffer_view;

    private:
        // Internal members...
    };

} // Rc::Render