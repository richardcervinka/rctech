#pragma once

#include "texture.h"
#include "buffer_linear_allocator.h"
#include "descriptor_heap.h"
#include "pipeline_state.h"
#include "fence.h"
#include "command_buffer.h"
#include "device.h"

namespace Rc::Render
{   
    class Frame
    {
    public:
        static constexpr uint64_t staging_buffer_size = 2048;
        static constexpr uint64_t uniform_buffer_size = 1024;

        void Create(Device const& device);

        void UpdateResourceDescriptorHeapBuffer(Device const& device);

        void Begin();

        // void End(SwapChain const& swap_chain);

        void BeginTestRenderPass(
            Pipeline const& pipeline,
            RenderTargetView const& framebuffer
        );

        void EndRenderPass();

        void Wait() const;

    private:
        friend class Renderer;

        std::unique_ptr<RenderCommandQueue> m_queue;

        std::unique_ptr<Fence> fence;
        
        std::unique_ptr<RenderCommandBuffer> commands;

        std::unique_ptr<BufferLinearAllocator> staging_buffer;

        std::unique_ptr<Buffer> uniform_buffer;

        std::unique_ptr<Buffer> resource_descriptor_heap_buffer;

        std::unique_ptr<ResourceDescriptorHeap> resource_descriptor_heap;
    };

} // Rc::Render