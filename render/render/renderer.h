#pragma once

#include "instance.h"
#include "platform/window.h"
#include "texture.h"
#include <array>
#include <functional>
#include "shader.h"
#include "descriptor_heap.h"
#include "core/camera.h"
#include "core/vertex.h"
#include "frame_renderer.h"
#include "buffer_linear_allocator.h"
#include "buffer_ring_allocator.h"
#include "resource.h"

struct TestModel
{
    Rc::Render::VertexBufferHandle vb_handle;
    Rc::Render::IndexBufferHandle ib_handle; 

    std::vector<Rc::Gfx::Transformations> instances;
};

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

        // TEST rendering -----------------------

        std::unique_ptr<TestModel> test_model;

        // --------------------------------------

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
            return swap_chain->Width();
        }

        // Get height of the associated swap chain.
        int Height() const
        {
            return swap_chain->Height();
        }

        // BeginFrame -> render commands -> EndFrame
        void BeginFrame();

        // BeginFrame -> render commands -> EndFrame
        void EndFrame();

        VertexBufferHandle AllocateVertexBuffer(ResourceFamily family, uint64_t size)
        {
            return resource_manager->AllocateVertexBuffer(family, size);
        }

        IndexBufferHandle AllocateIndexBuffer(ResourceFamily family, uint64_t size)
        {
            return resource_manager->AllocateIndexBuffer(family, size);
        }

        void BeginUpload()
        {
            auto lock = std::lock_guard{*resource_uploader};
            resource_uploader->BeginUpload();
        }

        void EndUpload()
        {
            auto lock = std::lock_guard{*resource_uploader};
            resource_uploader->EndUpload();
        }

        bool QueryUpload(uint64_t timeline) const
        {
            return resource_uploader->Complete(timeline);
        }
        
        uint64_t Upload(VertexBufferHandle handle, std::function<void(BufferWriter&)> writer_callback)
        {
            auto lock = std::lock_guard{*resource_uploader};
            return resource_uploader->Upload(resource_manager->GetBufferRegion(handle), writer_callback);
        }

        uint64_t Upload(IndexBufferHandle handle, std::function<void(BufferWriter&)> writer_callback)
        {
            auto lock = std::lock_guard{*resource_uploader};
            return resource_uploader->Upload(resource_manager->GetBufferRegion(handle), writer_callback);
        }

    private:
        // Assign vertex shader to the slot.
        void SetVertexShader(VertexShaderSlot slot, std::unique_ptr<Shader> shader)
        {
            vertex_shaders[static_cast<std::size_t>(slot)] = std::move(shader);
        }

        // Assign pixel shader to the slot.
        void SetPixelShader(PixelShaderSlot slot, std::unique_ptr<Shader> shader)
        {
            pixel_shaders[static_cast<std::size_t>(slot)] = std::move(shader);
        }

        VkShaderModule GetVertexShader(VertexShaderSlot slot)
        {
            return vertex_shaders[static_cast<std::size_t>(slot)]->Handle();
        }

        VkShaderModule GetPixelShader(PixelShaderSlot slot)
        {
            return pixel_shaders[static_cast<std::size_t>(slot)]->Handle();
        }

        void OnWindowSize(Window::EventSize::Payload const& e)
        {
            Resize(e.w, e.h);
        }

        std::unique_ptr<Instance> instance;
        
        std::unique_ptr<Device> device;

        std::unique_ptr<Surface> surface;

        std::unique_ptr<SwapChain> swap_chain;

        std::unique_ptr<RenderCommandQueue> render_queue;

        std::array<std::unique_ptr<Shader>, std::to_underlying(VertexShaderSlot::Count)> vertex_shaders;
        std::array<std::unique_ptr<Shader>, std::to_underlying(PixelShaderSlot::Count)> pixel_shaders;

        // Frames-In-Flight
        std::vector<Frame> frames;

        // Current render frame, updated by the BeginFrame()
        Frame* frame {nullptr};

        // Position in the m_frames. Updated by the EndFrame()
        uint64_t frame_number {0};

        std::shared_ptr<PipelineLayout> pipeline_layout;

        std::unique_ptr<Pipeline> test_pipeline;
        std::unique_ptr<Pipeline> test_vertex_pipeline;

        std::unique_ptr<ResourceManager> resource_manager;
        std::unique_ptr<ResourceUploader> resource_uploader;

        Window::EventSize::Handler on_window_size {this, &Renderer::OnWindowSize};

        // Create embedded shaders.
        void InitializeShaders();

        std::unique_ptr<Device> CreateDevice();

        void Test();
    };

} // Rc::Render