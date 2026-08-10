#pragma once

#include "texture.h"
#include "buffer_linear_allocator.h"
#include "descriptor_heap.h"
#include "pipeline_state.h"
#include "fence.h"
#include "command_buffer.h"
#include "device.h"
#include "core/camera.h"
#include "core/vertex.h"
#include "constants.h"

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
        static constexpr uint64_t staging_buffer_size = 2048;
        //static constexpr uint64_t render_pass_uniform_buffer_size = 16 * 4;

        void Create(Device const& device, uint32_t width, uint32_t height);

        void Resize(Device const& device, uint32_t width, uint32_t height);

        void UpdateResourceDescriptorHeap(Device const& device);

        void Begin(/*FrameData const& data*/);

        // void End(SwapChain const& swap_chain);

        void BeginTestRenderPass(
            Pipeline const& pipeline,
            RenderTargetView const& framebuffer,
            RenderPassContext const& context
        );

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

    private:
        friend class Renderer;

        std::unique_ptr<RenderCommandQueue> queue;

        std::unique_ptr<Fence> fence;
        
        std::unique_ptr<RenderCommandBuffer> commands;

        std::unique_ptr<BufferLinearAllocator> staging_buffer;

        std::unique_ptr<Buffer> instance_buffer;

        std::span<std::byte> instance_map;

        BufferWriter instance_writer;

        std::unique_ptr<Buffer> render_pass_uniform_buffer;

        // TODO: Frame uniform buffer

        std::unique_ptr<Buffer> resource_descriptor_heap_buffer;

        std::unique_ptr<ResourceDescriptorHeap> resource_descriptor_heap;

        std::unique_ptr<Texture2D> depth_buffer;
        std::unique_ptr<RenderTargetView> depth_buffer_view;
    };

} // Rc::Render