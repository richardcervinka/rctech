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

    static void SortAdapters(std::span<std::unique_ptr<Adapter>> adapters)
    {
        // Predicate l > r
        const auto predicate = [](std::unique_ptr<Adapter> const& l, std::unique_ptr<Adapter> const& r)
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
        };

        std::ranges::sort(adapters, predicate);
    }

    void Renderer::Initialize(Window& window)
    {
        m_instance = std::make_unique<Instance>();
        m_instance->EnableValidation();
        
        m_surface = m_instance->CreateSurface(window);

        // Get available GPUs
        auto adapters = m_instance->EnumerateAdapters();

        SortAdapters(adapters);

        for (auto& adapter : adapters)
        {
            if (adapter->ApiVersion() >= m_instance->ApiVersion())
            {
                try
                {
                    m_device = adapter->CreateDevice(*m_surface);

                    Str::From(adapter->ApiVersion());
                    //Log::Debug(std::format("Adapter {} vk:{}\n", adapter->Name(), Str::From(adapter->ApiVersion())));

                    break;
                }
                catch (std::exception const& e)
                {
                    Log::Error(std::format("Create device error: {}\n", e.what()));
                }
            }
        }

        if (m_device == nullptr)
        {
            throw std::runtime_error("No supported GPU found!");
        }

        m_swap_chain = m_device->CreateSwapChain(*m_surface, window);
        m_render_queue = m_device->CreateGraphicsQueue();

        // Create frames-in-flight
        m_frames.resize(m_swap_chain->Count());
        for (auto& frame : m_frames)
        {
            frame.fence = m_device->CreateFence();
            frame.render_commands = m_render_queue->CreateCommandBuffer();
            frame.staging_buffer = std::make_unique<BufferLinear>(m_device->AllocateStagingBuffer(2048 * 2048 * 4 * 8));
        }

        // Create swap chain image views
        for (int i = 0; i < m_swap_chain->Count(); i++)
        {
            m_back_buffers.emplace_back(m_swap_chain->GetImage(i).CreateView());
        }

        // Create embedded shaders.
        InitializeShaders();

        // Handle window resizing.
        window.OnEventSize(m_on_window_size);

        // ---------------------------- TEST

        ReserveVertexBuffer(Usage::Permanent, 2048);
        ReserveIndexBuffer(Usage::Permanent, 256);


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
        m_index_buffer = std::make_unique<BufferLinear>(m_device->AllocateIndexBuffer(capacity));
    }

    BufferHandle Renderer::AllocateIndexbuffer(Usage usage, uint64_t size)
    {
        assert(m_index_buffer != nullptr);

        return {m_index_buffer->Allocate(size), usage, 0xFFFF};
    }

    uint64_t Renderer::GetIndexBufferCapacity(Usage usage) const
    {
        if (m_index_buffer == nullptr)
        {
            return 0;
        }
        return m_index_buffer->Capacity();
    }

    uint64_t Renderer::GetIndexBufferAvailable(Usage usage) const
    {
        if (m_index_buffer == nullptr)
        {
            return 0;
        }
        return m_index_buffer->Available();
    }

    void Renderer::ReserveVertexBuffer(Usage usage, uint64_t capacity)
    {
        m_vertex_buffer = std::make_unique<BufferLinear>(m_device->AllocateVertexBuffer(capacity));
    }

    BufferHandle Renderer::AllocateVertexbuffer(Usage usage, uint64_t size)
    {
        assert(m_vertex_buffer != nullptr);

        return {m_vertex_buffer->Allocate(size), usage, 0xFFFF};
    }

    uint64_t Renderer::GetVertexBufferCapacity(Usage usage) const
    {
        if (m_vertex_buffer == nullptr)
        {
            return 0;
        }
        return m_vertex_buffer->Capacity();
    }

    uint64_t Renderer::GetVertexBufferAvailable(Usage usage) const
    {
        if (m_vertex_buffer == nullptr)
        {
            return 0;
        }
        return m_vertex_buffer->Available();
    }

    void Renderer::BeginFrame()
    {
        auto& frame = m_frames[m_frame_number % m_frames.size()];

        frame.fence->Wait();

        frame.render_commands->Reset();
        frame.staging_buffer->Reset();
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

        // Fill staging buffer
        
        auto vb_region = frame.staging_buffer->Allocate(Gfx::VertexBasic::stride * 4);
        auto vb_data = frame.staging_buffer->Map<Gfx::VertexBasic>(vb_region);

        vb_data[0].position = {-0.7, -0.7, 0};
        vb_data[1].position = {0.7, 0.7, 0};
        vb_data[2].position = {-0.7, 0.7, 0};
        vb_data[3].position = {0.7, -0.7, 0};

        vb_data[0].color = {1, 0, 0};
        vb_data[1].color = {0, 1, 0};
        vb_data[2].color = {0, 0, 1};
        vb_data[3].color = {1, 1, 1};

        auto ib_region = frame.staging_buffer->Allocate(sizeof(uint16_t) * 6);
        auto ib_data = frame.staging_buffer->Map<uint16_t>(ib_region);

        ib_data[0] = 0;
        ib_data[1] = 1;
        ib_data[2] = 2;
        ib_data[3] = 0;
        ib_data[4] = 3;
        ib_data[5] = 1;
        
        frame.render_commands->TransferBuffer(frame.staging_buffer->GetBuffer(), m_vertex_buffer->GetBuffer(), vb_region.Offset(), 0, vb_region.Size());
        frame.render_commands->TransferBuffer(frame.staging_buffer->GetBuffer(), m_index_buffer->GetBuffer(), ib_region.Offset(), 0, ib_region.Size());
        frame.render_commands->UseVertexBuffer(m_vertex_buffer->GetBuffer(), 0, vb_region.Size());
        frame.render_commands->UseIndexBuffer(m_index_buffer->GetBuffer(), 0, ib_region.Size());
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