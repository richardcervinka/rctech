#pragma once

#include "instance.h"
#include "platform/window.h"
#include "texture.h"
#include <array>
#include "shader.h"
#include "buffer_linear.h"
#include "descriptor_heap.h"
#include "core/camera.h"

namespace Rc::Render
{
    enum class VertexShaderSlot
    {
        Null,
        Test,
        Overlay,

        Count
    };

    enum class PixelShaderSlot
    {
        Null,

        Count
    };

    // Resource lifetime.
    enum class Usage
    {
        Permanent,
        Static,
        Dynamic
    };

    class BufferHandle
    {
    public:
        BufferHandle() = default;

    private:
        friend class Renderer;

        BufferHandle(BufferRegion region, Usage usage, uint64_t uid) :
            m_region{region},
            m_usage{usage},
            m_uid{uid}
        {}

        BufferRegion m_region;
        Usage m_usage {};
        uint64_t m_uid {};
    };
    
    class Frame
    {
    public:
        void Begin()
        {
            fence->Wait();
            fence->Reset();
            staging_buffer->Reset();
            
            commands->Reset();
            commands->Begin();
            commands->BindResourceDescriptorHeap(*resource_descriptor_heap);
        }

        void BeginRenderPass(Pipeline const& pipeline, SwapChain const& swap_chain)
        {
            // Framebuffer memory barrier.
            commands->UseRenderingFramebuffer(swap_chain.GetRenderTargetView());

            RenderTargetAttachments attachments;
            attachments.EnableColorAttachment(RenderTargetSlot::FrameBuffer, swap_chain.GetRenderTargetView());
            attachments.ClearRenderTarget(RenderTargetSlot::FrameBuffer, Color(0, 0, 0, 1));

            commands->BindPipeline(pipeline);
            commands->BeginRendering(swap_chain.RenderArea(), attachments);
        }

        void EndRenderPass()
        {
            commands->EndRendering();
        }

    private:
        friend class Renderer;

        std::unique_ptr<Fence> fence;
        
        std::unique_ptr<RenderCommandBuffer> commands;

        std::unique_ptr<BufferLinear> staging_buffer;

        std::unique_ptr<Buffer> uniform_buffer;

        std::unique_ptr<Buffer> resource_descriptor_heap_buffer;

        std::unique_ptr<ResourceDescriptorHeap> resource_descriptor_heap;
    };

    //
    // Main rendering abstraction layer
    //
    class Renderer
    {
    public:
        Renderer();
        ~Renderer();

        Renderer(Renderer const&) = delete;
        Renderer& operator=(Renderer const&) = delete;
        Renderer(Renderer&& other) = delete;
        Renderer& operator=(Renderer&& other) = delete;

        void Initialize(Window& window);

        void Resize(int width, int height);

        // Get width of the associated swap chain.
        int Width() const
        {
            return m_swap_chain->Width();
        }

        // Get height of the associated swap chain.
        int Height() const
        {
            return m_swap_chain->Height();
        }

        // BeginFrame -> render commands -> EndFrame
        void BeginFrame();

        // BeginFrame -> render commands -> EndFrame
        void EndFrame();

        void ReserveIndexBuffer(Usage usage, uint64_t capacity);
        BufferHandle AllocateIndexbuffer(Usage usage, uint64_t size);
        uint64_t GetIndexBufferCapacity(Usage usage) const; // Return BufferInfo {capacity, available...}
        uint64_t GetIndexBufferAvailable(Usage usage) const;

        void ReserveVertexBuffer(Usage usage, uint64_t capacity);
        BufferHandle AllocateVertexbuffer(Usage usage, uint64_t size);
        uint64_t GetVertexBufferCapacity(Usage usage) const;
        uint64_t GetVertexBufferAvailable(Usage usage) const;

        //void TransferBuffer(BufferHandle)

        // Test interace

        void SetCamera(Gfx::PerspectiveCamera const& camera);

    private:
        void InitializeFramesInFlight();

        // Assign vertex shader to the slot.
        void SetVertexShader(VertexShaderSlot slot, std::unique_ptr<Shader> shader)
        {
            m_vertex_shaders[static_cast<std::size_t>(slot)] = std::move(shader);
        }

        // Assign pixel shader to the slot.
        void SetPixelShader(PixelShaderSlot slot, std::unique_ptr<Shader> shader)
        {
            m_pixel_shaders[static_cast<std::size_t>(slot)] = std::move(shader);
        }

        VkShaderModule GetVertexShader(VertexShaderSlot slot)
        {
            return m_vertex_shaders[static_cast<std::size_t>(slot)]->Handle();
        }

        VkShaderModule GetPixelShader(PixelShaderSlot slot)
        {
            return m_pixel_shaders[static_cast<std::size_t>(slot)]->Handle();
        }

        void OnWindowSize(Window::EventSize::Payload const& e)
        {
            Resize(e.w, e.h);
        }

        std::unique_ptr<Instance> m_instance;
        
        std::unique_ptr<Device> m_device;

        std::unique_ptr<Surface> m_surface;

        std::unique_ptr<SwapChain> m_swap_chain;

        std::unique_ptr<RenderCommandQueue> m_render_queue;

        std::unique_ptr<BufferLinear> m_transfer_buffer;
        std::unique_ptr<TransferCommandQueue> m_transfer_queue;
        std::unique_ptr<TransferCommandBuffer> m_transfer_ocmmands;
        std::unique_ptr<TimelineSemaphore> m_transfer_semaphore;

        std::array<std::unique_ptr<Shader>, static_cast<int>(VertexShaderSlot::Count)> m_vertex_shaders;
        std::array<std::unique_ptr<Shader>, static_cast<int>(PixelShaderSlot::Count)> m_pixel_shaders;

        // Frames-In-Flight
        std::vector<Frame> m_frames;

        Frame* m_frame {nullptr};

        uint64_t m_frame_number {0};

        std::shared_ptr<PipelineLayout> m_pipeline_layout;

        std::unique_ptr<Pipeline> m_test_pipeline;
        std::unique_ptr<Pipeline> m_test_vertex_pipeline;

        std::unique_ptr<BufferLinear> m_vertex_buffer; //------------------------- TEST
        std::unique_ptr<BufferLinear> m_index_buffer; //------------------------- TEST: Static index buffer

        Window::EventSize::Handler m_on_window_size {this, &Renderer::OnWindowSize};

        // Create embedded shaders.
        void InitializeShaders();

        std::unique_ptr<Device> CreateDevice();

        // TEST rendering -----------------------

        Matrix4<double> m_camera_projection {Matrix4<double>::Identity()}; // -------------- TODO: Reset each frame

        void Test();
    };

} // Rc::Render