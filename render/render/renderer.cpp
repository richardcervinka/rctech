#include "renderer.h"
#include "index_buffer.h"
#include "resources.h"
#include <cstdint>
#include <span>
#include <stdexcept>
#include "platform/log.h"
#include "core/vertex.h" //----------
#include "vertex_traits.h"

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

        m_staging_buffer = nullptr;
        m_vertex_buffer = nullptr;
        m_index_buffer = nullptr;
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

    void Renderer::Initialize(Window& window)
    {
        m_instance = std::make_unique<Instance>();
        m_instance->EnableValidation();
        
        m_surface = m_instance->CreateSurface(window);

        // Get available GPUs
        auto adapters = m_instance->EnumerateAdapters();

        // Print adapters
        for (auto const& adapter : adapters)
        {
            Log::Debug(std::format("Adapter {}\n", adapter.GetName()));
        }

        m_device = adapters.back().CreateDevice(*m_surface);
        m_swap_chain = m_device->CreateSwapChain(*m_surface, window);
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

        // Create embedded shaders.
        SetVertexShader(VertexShaderSlot::Null, m_device->CreateShader(Res::Vs::Dummy()));
        SetVertexShader(VertexShaderSlot::Test, m_device->CreateShader(Res::Vs::Test()));
        SetVertexShader(VertexShaderSlot::Overlay, m_device->CreateShader(Res::Vs::Overlay()));
        SetPixelShader(PixelShaderSlot::Null, m_device->CreateShader(Res::Ps::Dummy()));

        m_staging_buffer = std::make_unique<StagingBuffer>(m_device->AllocateStagingBuffer(2048 * 2048 * 4 * 8));

        window.OnEventSize(m_on_window_size);

        // ---------------------------- TEST

        ReserveVertexBuffer(Usage::Permanent, 2048);
        ReserveIndexBuffer(Usage::Permanent, 256);

        {
            auto buffer = m_staging_buffer->Data();

            auto* pv = reinterpret_cast<Gfx::VertexBasic*>(buffer.data());

            pv[0].position = {-0.7, -0.7, 0};
            pv[1].position = {0.7, 0.7, 0};
            pv[2].position = {-0.7, 0.7, 0};
            pv[3].position = {0.7, -0.7, 0};

            pv[0].color = {1, 0, 0};
            pv[1].color = {0, 1, 0};
            pv[2].color = {0, 0, 1};
            pv[3].color = {1, 1, 1};

            auto* ib = reinterpret_cast<uint16_t*>(buffer.data() + 128);
            ib[0] = 0;
            ib[1] = 1;
            ib[2] = 2;
            ib[3] = 0;
            ib[4] = 3;
            ib[5] = 1;
        }

        m_pipeline_layout = m_device->CreatePipelineLayout();
        
        auto pipeline_factory = m_device->CreatePipelineFactory();
        pipeline_factory.SetPipelineLayout(m_pipeline_layout);

        pipeline_factory.SetVertexShader(GetVertexShader(VertexShaderSlot::Overlay));
        pipeline_factory.SetPixelShader(GetPixelShader(PixelShaderSlot::Null));
        pipeline_factory.SetVertexInputAttributes({});
        pipeline_factory.SetVertexInputBinding(0);
        m_test_pipeline = pipeline_factory.Create();

        pipeline_factory.SetVertexShader(GetVertexShader(VertexShaderSlot::Test));
        pipeline_factory.SetPixelShader(GetPixelShader(PixelShaderSlot::Null));
        pipeline_factory.SetVertexInputBinding(Gfx::VertexBasic::stride);
        pipeline_factory.SetVertexInputAttributes(Traits<Gfx::VertexBasic>::attributes);
        m_test_vertex_pipeline = pipeline_factory.Create();

        Log::Debug("Renderer initialized");
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

    void Renderer::ReserveIndexBuffer(Usage usage, uint64_t capacity)
    {
        m_index_buffer = std::make_unique<LinearBuffer>(m_device->AllocateIndexBuffer(capacity));
    }

    void Renderer::FreeIndexBuffer(Usage usage)
    {
        m_index_buffer = nullptr;
    }

    BufferHandle Renderer::AllocateIndexbuffer(Usage usage, uint64_t size)
    {
        assert(m_index_buffer != nullptr);

        return {m_index_buffer->Pop(size), usage, 0xFFFF};
    }

    void Renderer::ReserveVertexBuffer(Usage usage, uint64_t capacity)
    {
        m_vertex_buffer = std::make_unique<LinearBuffer>(m_device->AllocateVertexBuffer(capacity));
    }

    void Renderer::FreeVertexBuffer(Usage usage)
    {
        m_vertex_buffer = nullptr;
    }

    BufferHandle Renderer::AllocateVertexbuffer(Usage usage, uint64_t size)
    {
        assert(m_vertex_buffer != nullptr);

        return {m_vertex_buffer->Pop(size), usage, 0xFFFF};
    }

    void Renderer::BeginFrame()
    {
        auto& frame = m_frames[m_frame_number % m_frames.size()];

        frame.fence->Wait();

        frame.render_commands->Reset();

        frame.render_commands->Begin();

        Test(frame);
    }

    void Renderer::EndFrame()
    {
        auto& frame = m_frames[m_frame_number % m_frames.size()];

        frame.render_commands->BeginPresentingFramebuffer(m_swap_chain->GetImage());

        frame.render_commands->End();

        m_render_queue->Submit(*frame.render_commands, *frame.fence);

        m_render_queue->Present(*m_swap_chain); //--------------------------------- not here

        m_frame_number += 1;
    }

    void Renderer::Test(Frame& frame)
    {
        auto back_buffer_index = m_swap_chain->AcquireNextImage();

        frame.render_commands->TransferBuffer(m_staging_buffer->GetBuffer(), m_vertex_buffer->GetBuffer(), 0, 0, Gfx::VertexBasic::stride * 4ull);
        frame.render_commands->TransferBuffer(m_staging_buffer->GetBuffer(), m_index_buffer->GetBuffer(), 128, 0, Gfx::VertexBasic::stride * 6ull);
        frame.render_commands->UseVertexBuffer(m_vertex_buffer->GetBuffer(), 0, Gfx::VertexBasic::stride * 4ull);
        frame.render_commands->UseIndexBuffer(m_index_buffer->GetBuffer(), 0, sizeof(uint16_t) * 4);
        frame.render_commands->BindVertexBuffer(m_vertex_buffer->GetBuffer(), 0);
        frame.render_commands->BindIndexBuffer(m_index_buffer->GetBuffer(), IndexType::Uint16, 0);
        frame.render_commands->BeginRenderingFramebuffer(m_swap_chain->GetImage());
        frame.render_commands->SetRenderTargetsCount(1);
        frame.render_commands->AttachRenderTarget(0, *m_back_buffers.at(back_buffer_index));
        frame.render_commands->ClearRenderTarget(0, Color(0, 0, 0, 1));
        frame.render_commands->BeginRendering({0, 0, m_swap_chain->Width(), m_swap_chain->Height()});
        frame.render_commands->BindPipeline(*m_test_vertex_pipeline);
        frame.render_commands->Test({0, 0, m_swap_chain->Width(), m_swap_chain->Height()});
        frame.render_commands->DrawIndexed(6, 1, 0, 0, 0);
        frame.render_commands->EndRendering();
    }

} // Rc::Render