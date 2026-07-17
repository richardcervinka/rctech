#include "renderer.h"
#include "index_buffer.h"
#include "resources.h"
#include <cstdint>
#include <cassert>
#include <algorithm>
#include <span>
#include <iostream>
#include <stdexcept>
#include "platform/log.h"
#include "core/vertex.h"
#include "base/math.h"
#include "core/transformations.h"
#include "core/camera.h"
#include "generic/input.h"

namespace Rc::Render
{
    Renderer::Renderer()
    {
    }

    Renderer::~Renderer()
    {
        if (device)
        {
            device->WaitIdle();
        }

        resource_manager = nullptr;
        test_vertex_pipeline = nullptr;
        test_pipeline = nullptr;
        pipeline_layout = nullptr;
        frames.clear();

        for (auto& shader : pixel_shaders)
        {
            shader = nullptr;
        }
        for (auto& shader : vertex_shaders)
        {
            shader = nullptr;
        }

        render_queue = nullptr;
        swap_chain = nullptr;
        surface = nullptr;
        device = nullptr;
        instance = nullptr;
    }

    static void SortAdapters(std::span<std::unique_ptr<Adapter>> adapters)
    {
        // Predicate l > r
        std::ranges::sort(adapters, [](auto const& l, auto const& r)
        {
            if (l->Integrated() && r->Discrete())
            {
                return false;
            }
            if (l->Discrete() && r->Integrated())
            {
                return true;
            }
            return l->DeviceLocalMemory() > r->DeviceLocalMemory();
        });
    }

    std::unique_ptr<Device> Renderer::CreateDevice()
    {
        // Get available GPUs
        auto adapters = instance->EnumerateAdapters();

        SortAdapters(adapters);
        
        for (auto& adapter : adapters)
        {
            if (adapter->ApiVersion() < instance->ApiVersion())
            {
                continue;
            }
            try
            {
                Log::Debug(std::format("Adapter {} vulkan {}\n", adapter->Name(), Str::From(adapter->ApiVersion())));
                
                auto device = adapter->CreateDevice(*surface);

                return device;
            }
            catch (std::exception const& e)
            {
                Log::Error(std::format("Create device error: {}\n", e.what()));
            }
        }

        throw std::runtime_error("No supported GPU found!");
    }

    void Renderer::Initialize(Window& window)
    {
        instance = std::make_unique<Instance>();
        instance->EnableValidation();
        
        // Core objects.
        surface = instance->CreateSurface(window);
        device = CreateDevice();
        swap_chain = device->CreateSwapChain(*surface, window);
        render_queue = device->CreateGraphicsQueue();
        pipeline_layout = device->CreatePipelineLayout();

        // Create frames in flight.
        frames.resize(swap_chain->Size());
        for (auto& frame : frames)
        {
            frame.Create(*device);
            frame.UpdateResourceDescriptorHeap(*device);
        }

        // Create embedded shaders.
        InitializeShaders();

        // Handle window resizing.
        window.OnEventSize(on_window_size);

        resource_manager = std::make_unique<ResourceManager>(device);
        
        // ---------------------------- TEST ----------------------------

        resource_manager->ReserveVertexBuffer(ResourceFamily{0}, 2048);
        resource_manager->ReserveIndexBuffer(ResourceFamily{0}, 2048);
        resource_manager->ReserveInstanceBuffer(ResourceFamily{0}, 2048);

        {
            auto factory = device->CreatePipelineFactory();
            factory.SetPipelineLayout(pipeline_layout);
            factory.SetVertexShader(GetVertexShader(VertexShaderSlot::Overlay));
            factory.SetPixelShader(GetPixelShader(PixelShaderSlot::Null));
            test_pipeline = factory.Create();
        }
        {
            auto factory = device->CreatePipelineFactory();
            factory.SetPipelineLayout(pipeline_layout);
            factory.SetVertexShader(GetVertexShader(VertexShaderSlot::Test));
            factory.SetPixelShader(GetPixelShader(PixelShaderSlot::Null));
            factory.SetVertexBinding(Gfx::VertexBinding::PerVertex, sizeof(Gfx::VertexBasic));
            factory.SetVertexBinding(Gfx::VertexBinding::PerInstance, sizeof(Gfx::VertexInstance));
            factory.SetVertexAttributes({Gfx::VertexBasic::attributes, Gfx::VertexInstance::attributes});
            test_vertex_pipeline = factory.Create();
        }

        Log::Debug("Renderer initialized");
    }

    void Renderer::InitializeShaders()
    {
        SetVertexShader(VertexShaderSlot::Null, device->CreateShader(Res::Vs::Dummy()));
        SetVertexShader(VertexShaderSlot::Test, device->CreateShader(Res::Vs::Test()));
        SetVertexShader(VertexShaderSlot::Overlay, device->CreateShader(Res::Vs::Overlay()));
        SetPixelShader(PixelShaderSlot::Null, device->CreateShader(Res::Ps::Dummy()));
    }

    void Renderer::Resize(int width, int height)
    {
        device->WaitIdle();
        swap_chain->Resize(width, height);
    }

    void Renderer::BeginFrame()
    {
        frame = &frames[frame_number % frames.size()];

        frame->Begin();

        swap_chain->AcquireNextImage();

        if (test_model != nullptr)
        {
            Test();
        }
    }

    void Renderer::EndFrame()
    {
        frame->commands->UsePresentingFramebuffer(swap_chain->GetRenderTargetView()); // ---------------
        frame->commands->End();
        
        render_queue->WaitSemaphore(swap_chain->GetAcquireSemaphore());
        render_queue->SignalSemaphore(swap_chain->GetPresentSemaphore());
        render_queue->Submit(*frame->commands, *frame->fence);

        swap_chain->Present(*render_queue);

        frame_number += 1;
    }

    void Renderer::Test()
    {
        static Gfx::PerspectiveCamera camera;

        camera.transformations.z = -3.0;
        camera.fov = Math::DegToRad(75.0);

        if (Input::Pushed(Input::KeyCode::LeftArrow))
        {
            camera.transformations.x += 0.01; 
        }
        if (Input::Pushed(Input::KeyCode::RightArrow))
        {
            camera.transformations.x -= 0.01; 
        }
        if (Input::Pushed(Input::KeyCode::UpArrow))
        {
            camera.transformations.y += 0.01; 
        }
        if (Input::Pushed(Input::KeyCode::DownArrow))
        {
            camera.transformations.y -= 0.01; 
        }
        if (Input::Pushed(Input::KeyCode::D))
        {
            camera.transformations.yaw += 0.005; 
        }
        if (Input::Pushed(Input::KeyCode::A))
        {
            camera.transformations.yaw -= 0.005; 
        }
        if (Input::Pushed(Input::KeyCode::W))
        {
            camera.transformations.pitch += 0.005; 
        }
        if (Input::Pushed(Input::KeyCode::S))
        {
            camera.transformations.pitch -= 0.005; 
        }

        RenderPassContext render_pass_context {};
        render_pass_context.camera = &camera;

        frame->commands->UseVertexBuffer(resource_manager->GetBufferRegion(test_model->in_handle));
        frame->commands->UseVertexBuffer(resource_manager->GetBufferRegion(test_model->vb_handle));
        frame->commands->UseIndexBuffer(resource_manager->GetBufferRegion(test_model->ib_handle));

        //m_swap_chain->AcquireNextImage();

        frame->BeginTestRenderPass(
            *test_vertex_pipeline,
            swap_chain->GetRenderTargetView(),
            render_pass_context
        );

        frame->BindVertexBuffer(resource_manager->GetVertexBuffer(ResourceFamily{0}), 0, 0); // ---------------------- Use VertexBinding !!!!!!!!
        frame->BindVertexBuffer(resource_manager->GetInstanceBuffer(ResourceFamily{0}), 1, 0);
        frame->BindIndexBuffer(resource_manager->GetIndexBuffer(ResourceFamily{0}), IndexType::Uint16, 0);

        frame->Draw(36, 1, 0, 0, 0);

        frame->EndRenderPass();
    }

} // Rc::Render