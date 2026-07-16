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
#include "resource_manager.h"

struct TestModel
{
    Rc::Render::VertexBufferHandle vb_handle;
    Rc::Render::InstanceBufferHandle in_handle;
    Rc::Render::IndexBufferHandle ib_handle; 
};

inline TestModel g_test_model;

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
        uint64_t transfer_timeline {0};
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

        std::shared_ptr<ResourceManager> GetResourceManager()
        {
            return resource_manager;
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
        
        std::shared_ptr<Device> device;

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

        std::shared_ptr<ResourceManager> resource_manager;

        Window::EventSize::Handler on_window_size {this, &Renderer::OnWindowSize};

        // Create embedded shaders.
        void InitializeShaders();

        std::unique_ptr<Device> CreateDevice();

        void Test();
    };

} // Rc::Render