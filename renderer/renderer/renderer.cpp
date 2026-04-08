#include "renderer.h"
#include "resources.h"
#include <iostream>
#include <span>
#include "platform/log.h"

namespace Rc
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

        m_staging_buffer = nullptr;
        m_vertex_buffer = nullptr;
        m_test_vertex_pipeline = nullptr;
        m_test_pipeline = nullptr;
        m_pipeline_layout = nullptr;
        m_back_buffers.clear();
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

    static void PrintAdapters(std::span<Adapter const> adapters)
    {
        for (auto const& adapter : adapters)
        {
            Log::Debug(std::format("Adapter {}\n", adapter.GetName()));
        }
    }

    void Renderer::Initialize(Window& wnd)
    {
        m_instance = std::make_unique<Instance>();
        m_instance->EnableValidation();

        m_surface = m_instance->CreateSurface(wnd);

        // Get available GPUs
        auto adapters = m_instance->EnumerateAdapters();

        PrintAdapters(adapters);

        m_device = adapters.back().CreateDevice(*m_surface);
        m_swap_chain = m_device->CreateSwapChain(*m_surface, wnd);
        m_render_queue = m_device->CreateGraphicsQueue();

        // Create primary command buffer for each image in the swap chain.
        for (int i = 0; i < m_swap_chain->Count(); i++)
        {
            m_frames.emplace_back(m_device->CreateFence(), m_render_queue->CreateCommandBuffer());
        }

        // Create swap chain image views
        for (int i = 0; i < m_swap_chain->Count(); i++)
        {
            m_back_buffers.emplace_back(m_swap_chain->GetImage(i).CreateView());
        }

        // Create embedded shaders
        SetVertexShader(VertexShaderSlot::Null, m_device->CreateShader(Res::Vs::Dummy()));
        SetVertexShader(VertexShaderSlot::Test, m_device->CreateShader(Res::Vs::Test()));
        SetVertexShader(VertexShaderSlot::Overlay, m_device->CreateShader(Res::Vs::Overlay()));
        SetPixelShader(PixelShaderSlot::Null, m_device->CreateShader(Res::Ps::Dummy()));

        m_staging_buffer = m_device->AllocateStagingBuffer(1024 * 1024 * 4);

        wnd.OnEventSize(m_on_window_size);

        // ---------------------------- TEST

        m_vertex_buffer = m_device->AllocateVertexBuffer(256);

        {
            struct Vert
            {
                Float3 position;
                Float3 color;
            };

            auto buffer = m_staging_buffer->Data();

            Vert* pv = reinterpret_cast<Vert*>(buffer.data());

            pv[0].position = Float3(0.0, -0.7, 0);
            pv[1].position = Float3(0.7, 0.7, 0);
            pv[2].position = Float3(-0.7, 0.7, 0);

            pv[0].color = Float3(1, 0, 0);
            pv[1].color = Float3(0, 1, 0);
            pv[2].color = Float3(0, 0, 1);
        }

        m_pipeline_layout = m_device->CreatePipelineLayout();
        
        auto pipeline_factory = m_device->CreatePipelineFactory();
        pipeline_factory.SetPipelineLayout(m_pipeline_layout);

        pipeline_factory.SetVertexShader(GetVertexShader(VertexShaderSlot::Overlay));
        pipeline_factory.SetPixelShader(GetPixelShader(PixelShaderSlot::Null));
        pipeline_factory.SetVertexInputRate(0);
        pipeline_factory.SetVertexInputAttributes({});
        m_test_pipeline = pipeline_factory.Create();

        pipeline_factory.SetVertexShader(GetVertexShader(VertexShaderSlot::Test));
        pipeline_factory.SetPixelShader(GetPixelShader(PixelShaderSlot::Null));
        pipeline_factory.SetVertexInputRate(VertexBasic::stride);
        pipeline_factory.SetVertexInputAttributes(Traits<VertexBasic>::attributes);
        m_test_vertex_pipeline = pipeline_factory.Create();

        Log::Debug("Renderer initialized\n");
    }

    void Renderer::Resize(int width, int height)
    {
        m_device->WaitIdle();
        m_back_buffers.clear();
        m_swap_chain->Resize(width, height);

        // Create swap chain image views
        for (int i = 0; i < m_swap_chain->Count(); i++)
        {
            m_back_buffers.push_back(m_swap_chain->GetImage(i).CreateView());
        }
    }

    void Renderer::BeginFrame()
    {
        auto& frame = m_frames[m_frame % m_frames.size()];

        frame.fence->Wait();

        frame.render_commands->Reset();

        frame.render_commands->Begin();

        Test();
    }

    void Renderer::EndFrame()
    {
        auto& frame = m_frames[m_frame % m_frames.size()];

        frame.render_commands->BarrierPresentFramebuffer(m_swap_chain->GetImage());

        frame.render_commands->End();

        m_render_queue->Submit(*frame.render_commands, *frame.fence);

        m_render_queue->Present(*m_swap_chain); //--------------------------------- not here

        m_frame += 1;
    }

    void Renderer::Test()
    {
        auto& frame = m_frames[m_frame % m_frames.size()];
        
        auto back_buffer_index = m_swap_chain->AcquireNextImage();

        // Transfer test
        frame.render_commands->TransferBuffer(*m_staging_buffer, *m_vertex_buffer, 0, 4 * 3 * 2 * 3);

        frame.render_commands->BarrierRenderFramebuffer(m_swap_chain->GetImage());
        frame.render_commands->SetRenderTargetsCount(1);
        frame.render_commands->AttachRenderTarget(0, *m_back_buffers.at(back_buffer_index));
        frame.render_commands->ClearRenderTarget(0, Color(0, 0, 0, 1));
        frame.render_commands->BeginRendering({0, 0, m_swap_chain->Width(), m_swap_chain->Height()});
        frame.render_commands->BindPipeline(*m_test_pipeline);
        frame.render_commands->Test({0, 0, m_swap_chain->Width(), m_swap_chain->Height()});
        frame.render_commands->Draw(3, 1, 0, 0);
        frame.render_commands->EndRendering();
    }

} // Rc