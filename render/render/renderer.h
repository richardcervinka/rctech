#pragma once

#include "instance.h"
#include "platform/window.h"
#include "texture.h"
#include <array>
#include <functional>
#include "shader.h"
#include "descriptor_heap.h"
#include "core/camera.h"
#include "frame_renderer.h"
#include "buffer_linear_allocator.h"
#include "buffer_ring_allocator.h"
#include "buffer_manager.h"

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
        // Application data.
        Permanent,

        // Scene static data.
        Static,

        // ???
        Stream
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

        VertexBufferHandle AllocateVertexBuffer(ResourceFamilyName name, uint64_t size);
        IndexBufferHandle AllocateIndexBuffer(ResourceFamilyName name, uint64_t size);
        InstanceBufferHandle AllocateInstanceBuffer(ResourceFamilyName name, uint64_t size);

        void UploadBuffer(VertexBufferHandle handle, std::function<void(BufferWriter&)> writer_callback);
        void UploadBuffer(IndexBufferHandle handle, std::function<void(BufferWriter&)> writer_callback);
        void UploadBuffer(InstanceBufferHandle handle, std::function<void(BufferWriter&)> writer_callback);

        //void reserveInstanceBuffer

        // void TransferBuffer(BufferHandle)

        // Test interace

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

        void UploadBuffer(BufferRegion region, std::function<void(BufferWriter&)>& writer_callback);

        std::unique_ptr<Instance> m_instance;
        
        std::unique_ptr<Device> m_device;

        std::unique_ptr<Surface> m_surface;

        std::unique_ptr<SwapChain> m_swap_chain;

        std::unique_ptr<RenderCommandQueue> m_render_queue;

        std::unique_ptr<BufferRingAllocator> m_transfer_buffer; // TODO: Mozna vice bufferu pro ruzne velikosti chunku allocatoru
        std::unique_ptr<TransferCommandQueue> m_transfer_queue;
        std::unique_ptr<TransferCommandBuffer> m_transfer_commands;
        std::unique_ptr<TimelineSemaphore> m_transfer_semaphore;

        std::array<std::unique_ptr<Shader>, std::to_underlying(VertexShaderSlot::Count)> m_vertex_shaders;
        std::array<std::unique_ptr<Shader>, std::to_underlying(PixelShaderSlot::Count)> m_pixel_shaders;

        // Frames-In-Flight
        std::vector<Frame> m_frames;

        // Current render frame, updated by the BeginFrame()
        Frame* m_frame {nullptr};

        // Position in the m_frames. Updated by the EndFrame()
        uint64_t m_frame_number {0};

        std::shared_ptr<PipelineLayout> m_pipeline_layout;

        std::unique_ptr<Pipeline> m_test_pipeline;
        std::unique_ptr<Pipeline> m_test_vertex_pipeline;

        std::unique_ptr<BufferManager> m_buffer_manager;

        Window::EventSize::Handler m_on_window_size {this, &Renderer::OnWindowSize};

        // Create embedded shaders.
        void InitializeShaders();

        std::unique_ptr<Device> CreateDevice();

        // TEST rendering -----------------------

        VertexBufferHandle m_test_vb_handle;
        InstanceBufferHandle m_test_in_handle;
        IndexBufferHandle m_test_ib_handle;


        void Test();
    };

} // Rc::Render