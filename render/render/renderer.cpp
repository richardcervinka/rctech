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
        if (m_device)
        {
            m_device->WaitIdle();
        }

        m_resource_manager = nullptr;
        m_test_vertex_pipeline = nullptr;
        m_test_pipeline = nullptr;
        m_pipeline_layout = nullptr;
        m_frames.clear();

        for (auto& shader : m_pixel_shaders)
        {
            shader = nullptr;
        }
        for (auto& shader : m_vertex_shaders)
        {
            shader = nullptr;
        }

        m_render_queue = nullptr;
        m_swap_chain = nullptr;
        m_surface = nullptr;
        m_device = nullptr;
        m_instance = nullptr;
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
        auto adapters = m_instance->EnumerateAdapters();

        SortAdapters(adapters);
        
        for (auto& adapter : adapters)
        {
            if (adapter->ApiVersion() < m_instance->ApiVersion())
            {
                continue;
            }
            try
            {
                Log::Debug(std::format("Adapter {} vulkan {}\n", adapter->Name(), Str::From(adapter->ApiVersion())));
                
                auto device = adapter->CreateDevice(*m_surface);

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
        m_instance = std::make_unique<Instance>();
        m_instance->EnableValidation();
        
        // Core objects.
        m_surface = m_instance->CreateSurface(window);
        m_device = CreateDevice();
        m_swap_chain = m_device->CreateSwapChain(*m_surface, window);
        m_render_queue = m_device->CreateGraphicsQueue();
        m_pipeline_layout = m_device->CreatePipelineLayout();

        // Create frames in flight.
        m_frames.resize(m_swap_chain->Size());
        for (auto& frame : m_frames)
        {
            frame.Create(*m_device);
            frame.UpdateResourceDescriptorHeap(*m_device);
        }

        // Create embedded shaders.
        InitializeShaders();

        // Handle window resizing.
        window.OnEventSize(m_on_window_size);

        m_resource_manager = std::make_unique<ResourceManager>(m_device);
        
        // ---------------------------- TEST ----------------------------

        m_resource_manager->ReserveVertexBuffer(ResourceFamily{0}, 2048);
        m_resource_manager->ReserveIndexBuffer(ResourceFamily{0}, 2048);
        m_resource_manager->ReserveInstanceBuffer(ResourceFamily{0}, 2048);

        {
            auto factory = m_device->CreatePipelineFactory();
            factory.SetPipelineLayout(m_pipeline_layout);
            factory.SetVertexShader(GetVertexShader(VertexShaderSlot::Overlay));
            factory.SetPixelShader(GetPixelShader(PixelShaderSlot::Null));
            m_test_pipeline = factory.Create();
        }
        {
            auto factory = m_device->CreatePipelineFactory();
            factory.SetPipelineLayout(m_pipeline_layout);
            factory.SetVertexShader(GetVertexShader(VertexShaderSlot::Test));
            factory.SetPixelShader(GetPixelShader(PixelShaderSlot::Null));
            factory.SetVertexBinding(Gfx::VertexBinding::PerVertex, sizeof(Gfx::VertexBasic));
            factory.SetVertexBinding(Gfx::VertexBinding::PerInstance, sizeof(Gfx::VertexInstance));
            factory.SetVertexAttributes({Gfx::VertexBasic::attributes, Gfx::VertexInstance::attributes});
            m_test_vertex_pipeline = factory.Create();
        }

        Log::Debug("Renderer initialized");
    }

    void Renderer::InitializeShaders()
    {
        SetVertexShader(VertexShaderSlot::Null, m_device->CreateShader(Res::Vs::Dummy()));
        SetVertexShader(VertexShaderSlot::Test, m_device->CreateShader(Res::Vs::Test()));
        SetVertexShader(VertexShaderSlot::Overlay, m_device->CreateShader(Res::Vs::Overlay()));
        SetPixelShader(PixelShaderSlot::Null, m_device->CreateShader(Res::Ps::Dummy()));
    }

    void Renderer::Resize(int width, int height)
    {
        m_device->WaitIdle();
        m_swap_chain->Resize(width, height);
    }

    void Renderer::BeginFrame()
    {
        m_frame = &m_frames[m_frame_number % m_frames.size()];

        m_frame->Begin();

        m_resource_manager->QueryCounter();
        m_swap_chain->AcquireNextImage();

        if (m_resource_manager->Complete(transfer_timeline))
        {
            Test();
        }
    }

    void Renderer::EndFrame()
    {
        m_frame->commands->UsePresentingFramebuffer(m_swap_chain->GetRenderTargetView()); // ---------------
        m_frame->commands->End();
        
        m_render_queue->WaitSemaphore(m_swap_chain->GetAcquireSemaphore());
        m_render_queue->SignalSemaphore(m_swap_chain->GetPresentSemaphore());
        m_render_queue->Submit(*m_frame->commands, *m_frame->fence);

        m_swap_chain->Present(*m_render_queue);

        m_frame_number += 1;
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

        m_frame->commands->UseVertexBuffer(m_resource_manager->GetBufferRegion(g_test_model.in_handle));
        m_frame->commands->UseVertexBuffer(m_resource_manager->GetBufferRegion(g_test_model.vb_handle));
        m_frame->commands->UseIndexBuffer(m_resource_manager->GetBufferRegion(g_test_model.ib_handle));

        //m_swap_chain->AcquireNextImage();

        m_frame->BeginTestRenderPass(
            *m_test_vertex_pipeline,
            m_swap_chain->GetRenderTargetView(),
            render_pass_context
        );

        m_frame->BindVertexBuffer(m_resource_manager->GetVertexBuffer(ResourceFamily{0}), 0, 0); // ---------------------- Use VertexBinding !!!!!!!!
        m_frame->BindVertexBuffer(m_resource_manager->GetInstanceBuffer(ResourceFamily{0}), 1, 0);
        m_frame->BindIndexBuffer(m_resource_manager->GetIndexBuffer(ResourceFamily{0}), IndexType::Uint16, 0);

        m_frame->Draw(36, 1, 0, 0, 0);

        m_frame->EndRenderPass();
    }

} // Rc::Render