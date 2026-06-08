#pragma once

#include "instance.h"
#include "platform/window.h"
#include "texture.h"
#include <array>
#include "shader.h"

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
            return m_swap_chain->Width();
        }

        // BeginFrame -> render commands -> EndFrame
        void BeginFrame();

        // BeginFrame -> render commands -> EndFrame
        void EndFrame();

        void ReserveIndexBuffer(Usage usage, uint64_t capacity);
        void FreeIndexBuffer(Usage usage);
        BufferHandle AllocateIndexbuffer(Usage usage, uint64_t size);

        void ReserveVertexBuffer(Usage usage, uint64_t capacity);
        void FreeVertexBuffer(Usage usage);
        BufferHandle AllocateVertexbuffer(Usage usage, uint64_t size);

        //IndexBufferHandle CreateIndexBuffer(IndexType type, uint64_t size);

    private:
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

        vk::ShaderModule GetVertexShader(VertexShaderSlot slot)
        {
            return m_vertex_shaders[static_cast<std::size_t>(slot)]->Handle();
        }

        vk::ShaderModule GetPixelShader(PixelShaderSlot slot)
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

        std::unique_ptr<CommandQueue> m_render_queue;

        std::array<std::unique_ptr<Shader>, static_cast<int>(VertexShaderSlot::Count)> m_vertex_shaders;
        std::array<std::unique_ptr<Shader>, static_cast<int>(PixelShaderSlot::Count)> m_pixel_shaders;

        struct Frame
        {
            std::unique_ptr<Fence> fence;
            
            std::unique_ptr<CommandBuffer> render_commands;
        };

        // Frames-In-Flight
        std::vector<Frame> m_frames;

        uint64_t m_frame_number {0};

        std::vector<std::unique_ptr<TextureView2D>> m_back_buffers; //--------------------------- docasne ?

        std::shared_ptr<PipelineLayout> m_pipeline_layout;

        std::unique_ptr<Pipeline> m_test_pipeline;
        std::unique_ptr<Pipeline> m_test_vertex_pipeline;

        std::unique_ptr<StagingBuffer> m_staging_buffer;

        std::unique_ptr<LinearBuffer> m_vertex_buffer; //------------------------- TEST
        std::unique_ptr<LinearBuffer> m_index_buffer; //------------------------- TEST: Static index buffer

        Window::EventSize::Handler m_on_window_size {this, &Renderer::OnWindowSize};

        void Test(Frame& frame);
    };

} // Rc::Render