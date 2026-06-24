#include "renderer.h"
#include "index_buffer.h"
#include "resources.h"
#include <cstdint>
#include <span>
#include <stdexcept>
#include "platform/log.h"
#include "core/vertex.h"
#include "base/math.h"
#include "core/transformations.h"
#include "projection.h"

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
        m_transfer_buffer = nullptr;
        m_transfer_semaphore = nullptr;
        m_transfer_ocmmands = nullptr;
        m_transfer_queue = nullptr;
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

        // Transfer infrastructure.
        m_transfer_buffer = std::make_unique<BufferLinear>(m_device->AllocateStagingBuffer(1024));
        m_transfer_queue = m_device->CreateTransferQueue();
        m_transfer_ocmmands = m_transfer_queue->CreateCommandBuffer();
        m_transfer_semaphore = m_device->CreateTimelineSemaphore();

        // Create swap chain image views
        for (int i = 0; i < m_swap_chain->Count(); i++)
        {
            m_back_buffers.emplace_back(m_swap_chain->GetImage(i).CreateView());
        }

        InitializeFramesInFlight();

        // Create embedded shaders.
        InitializeShaders();

        // Handle window resizing.
        window.OnEventSize(m_on_window_size);

        // ---------------------------- TEST ----------------------------

        ReserveVertexBuffer(Usage::Permanent, 2048);
        ReserveIndexBuffer(Usage::Permanent, 256);

        auto pipeline_factory = m_device->CreatePipelineFactory();
        pipeline_factory.SetPipelineLayout(m_pipeline_layout);

        pipeline_factory.SetVertexShader(GetVertexShader(VertexShaderSlot::Overlay));
        pipeline_factory.SetPixelShader(GetPixelShader(PixelShaderSlot::Null));
        pipeline_factory.SetVertexInput(0, {});
        m_test_pipeline = pipeline_factory.Create();

        pipeline_factory.SetVertexShader(GetVertexShader(VertexShaderSlot::Test));
        pipeline_factory.SetPixelShader(GetPixelShader(PixelShaderSlot::Null));
        pipeline_factory.SetVertexInput(sizeof(Gfx::VertexBasic), Gfx::VertexBasic::attributes);
        m_test_vertex_pipeline = pipeline_factory.Create();

        // Test - transfer data do the vertex buffer
        {
            m_transfer_ocmmands->Reset();
            m_transfer_ocmmands->Begin();

            m_transfer_buffer->Reset();
            auto vb_region = m_transfer_buffer->Allocate(sizeof(Gfx::VertexBasic) * 8);
            auto vb_data = m_transfer_buffer->Map<Gfx::VertexBasic>(vb_region);

            vb_data[0].position = {-1.0f, -1.0f,  1.0f}; // front-left-bottom
            vb_data[1].position = { 1.0f,  1.0f,  1.0f}; // front-right-top
            vb_data[2].position = {-1.0f,  1.0f,  1.0f}; // front-left-top
            vb_data[3].position = { 1.0f, -1.0f,  1.0f}; // front-right-bottom
            vb_data[4].position = {-1.0f, -1.0f, -1.0f}; // back-left-bottom
            vb_data[5].position = { 1.0f,  1.0f, -1.0f}; // back-right-top
            vb_data[6].position = {-1.0f,  1.0f, -1.0f}; // back-left-top
            vb_data[7].position = { 1.0f, -1.0f, -1.0f}; // back-right-bottom

            vb_data[0].color = {1, 0, 0};
            vb_data[1].color = {1, 0, 0};
            vb_data[2].color = {1, 0, 0};
            vb_data[3].color = {1, 0, 0};
            vb_data[4].color = {0, 1, 0};
            vb_data[5].color = {0, 1, 0};
            vb_data[6].color = {0, 0, 1};
            vb_data[7].color = {0, 0, 1};

            auto ib_region = m_transfer_buffer->Allocate(sizeof(uint16_t) * 36);
            auto ib_data = m_transfer_buffer->Map<uint16_t>(ib_region);

            // front
            ib_data[0] = 0;
            ib_data[1] = 1;
            ib_data[2] = 2;
            ib_data[3] = 0;
            ib_data[4] = 3;
            ib_data[5] = 1;
            // back
            ib_data[6] = 4;
            ib_data[7] = 6;
            ib_data[8] = 5;
            ib_data[9] = 4;
            ib_data[10] = 5;
            ib_data[11] = 7;
            // left
            ib_data[12] = 4;
            ib_data[13] = 2;
            ib_data[14] = 6;
            ib_data[15] = 4;
            ib_data[16] = 0;
            ib_data[17] = 2;
            // right
            ib_data[18] = 3;
            ib_data[19] = 5;
            ib_data[20] = 1;
            ib_data[21] = 3;
            ib_data[22] = 7;
            ib_data[23] = 5;
            // top
            ib_data[24] = 2;
            ib_data[25] = 1;
            ib_data[26] = 5;
            ib_data[27] = 2;
            ib_data[28] = 5;
            ib_data[29] = 6;
            // bottom
            ib_data[30] = 4;
            ib_data[31] = 3;
            ib_data[32] = 0;
            ib_data[33] = 4;
            ib_data[34] = 7;
            ib_data[35] = 3;
            
            m_transfer_ocmmands->TransferBuffer(m_transfer_buffer->GetBuffer(), m_vertex_buffer->GetBuffer(), vb_region.Offset(), 0, vb_region.Size());
            m_transfer_ocmmands->TransferBuffer(m_transfer_buffer->GetBuffer(), m_index_buffer->GetBuffer(), ib_region.Offset(), 0, ib_region.Size());
        
            m_transfer_ocmmands->End();
            m_transfer_queue->Submit(*m_transfer_ocmmands, *m_transfer_semaphore);
            m_transfer_semaphore->Wait();
        }

        Log::Debug("Renderer initialized");
    }

    void Renderer::InitializeFramesInFlight()
    {
        assert(m_device != nullptr);
        assert(m_swap_chain != nullptr);
        assert(m_render_queue != nullptr);
        assert(m_transfer_buffer != nullptr);
        assert(m_transfer_ocmmands != nullptr);

        uint64_t const staging_buffer_size = 2048;
        uint64_t const uniform_buffer_size = 1024;

        // Prepare transfer buffer to transfer per-frame descriptor heaps.
        m_transfer_buffer->Reset();
        m_transfer_ocmmands->Reset();
        m_transfer_ocmmands->Begin();

        m_frames.resize(m_swap_chain->Count());

        for (auto& frame : m_frames)
        {
            frame.fence = m_device->CreateFence();
            frame.commands = m_render_queue->CreateCommandBuffer();
            frame.staging_buffer = std::make_unique<BufferLinear>(m_device->AllocateStagingBuffer(staging_buffer_size));
            frame.uniform_buffer = m_device->AllocateUniformBuffer(uniform_buffer_size);

            std::array<ResourceDescriptor, 1> const resource_descriptors
            {
                UniformBufferDescriptor
                {
                    .address = frame.uniform_buffer->Address(),
                    .size = frame.uniform_buffer->Size()
                }
            };

            frame.resource_descriptor_heap = m_device->CreateResourceDescriptorHeap(resource_descriptors);
            frame.resource_descriptor_heap_buffer = m_device->AllocateDescriptorHeapBuffer(frame.resource_descriptor_heap->SizeTotal());
            frame.resource_descriptor_heap->Attach(*frame.resource_descriptor_heap_buffer);

            // Transfer descriotor heap
            {
                auto region = m_transfer_buffer->Allocate(frame.resource_descriptor_heap->Size());
                auto memory = m_transfer_buffer->Map<std::byte>(region);
                frame.resource_descriptor_heap->Write(memory);

                m_transfer_ocmmands->TransferBuffer(
                    m_transfer_buffer->GetBuffer(),
                    *frame.resource_descriptor_heap_buffer,
                    region.Offset(),
                    0,
                    region.Size()
                );
            }
        }

        // Submit transfer commands.
        m_transfer_ocmmands->End();
        m_transfer_queue->Submit(*m_transfer_ocmmands, *m_transfer_semaphore);
        m_transfer_semaphore->Wait();
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

        frame.commands->Reset();
        frame.staging_buffer->Reset();
        frame.commands->Begin();

        Test(frame);
    }

    void Renderer::EndFrame()
    {
        auto& frame = m_frames[m_frame_number % m_frames.size()];

        frame.commands->BeginPresentingFramebuffer(m_swap_chain->GetImage());

        frame.commands->End();
        
        m_render_queue->WaitSemaphore(m_swap_chain->GetAcquireSemaphore());
        m_render_queue->SignalSemaphore(m_swap_chain->GetPresentSemaphore());
        m_render_queue->Submit(*frame.commands, *frame.fence);

        m_swap_chain->Present(*m_render_queue);

        m_frame_number += 1;
    }

    void Renderer::Test(Frame& frame)
    {
        static Transformations transformations;

        auto back_buffer_index = m_swap_chain->AcquireNextImage(); //----------------------- presunout

        // Write uniform buffer
        {
            transformations.yaw += 0.01;
            auto tm = transformations.GetTransformations();

            auto ub_region = frame.staging_buffer->Allocate(3 * (sizeof(float) * 16));
            auto ub_data = frame.staging_buffer->Map<float>(ub_region);
  
            auto projection_matrix = CreatePerspectiveProjectionMatrix(
                Width(),
                Height(),
                Math::DegToRad(90.0f),
                0.01,
                1000
            );

            Transformations camera_transformations;
            camera_transformations.z = -3;
            auto cm = camera_transformations.GetTransformations();

            projection_matrix.Store(ub_data);
            tm.Store(ub_data.subspan(16));
            cm.Store(ub_data.subspan(2 * 16));

            frame.commands->TransferBuffer(frame.staging_buffer->GetBuffer(), *frame.uniform_buffer, ub_region.Offset(), 0, ub_region.Size());
            frame.commands->UseUniformBuffer(*frame.uniform_buffer, frame.uniform_buffer->GetRegion());
        }

        frame.commands->UseVertexBuffer(m_vertex_buffer->GetBuffer(), m_vertex_buffer->GetBuffer().GetRegion());
        frame.commands->UseIndexBuffer(m_index_buffer->GetBuffer(), m_index_buffer->GetBuffer().GetRegion());
        frame.commands->UseResourceDescriptorHeapBuffer(*frame.resource_descriptor_heap_buffer, frame.resource_descriptor_heap_buffer->GetRegion());
        frame.commands->BindVertexBuffer(m_vertex_buffer->GetBuffer(), 0);
        frame.commands->BindIndexBuffer(m_index_buffer->GetBuffer(), IndexType::Uint16, 0);
        frame.commands->BindResourceDescriptorHeap(*frame.resource_descriptor_heap);
        frame.commands->BeginRenderingFramebuffer(m_swap_chain->GetImage());
        frame.commands->SetRenderTargetsCount(1);
        frame.commands->AttachRenderTarget(0, *m_back_buffers.at(back_buffer_index));
        frame.commands->ClearRenderTarget(0, Color(0, 0, 0, 1));
        frame.commands->BeginRendering({0, 0, m_swap_chain->Width(), m_swap_chain->Height()});
        frame.commands->BindPipeline(*m_test_vertex_pipeline);
        frame.commands->Test({0, 0, m_swap_chain->Width(), m_swap_chain->Height()});
        frame.commands->DrawIndexed(36, 1, 0, 0, 0);
        frame.commands->EndRendering();
    }

} // Rc::Render