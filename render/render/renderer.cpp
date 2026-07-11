#include "renderer.h"
#include "index_buffer.h"
#include "resources.h"
#include <cstdint>
#include <cassert>
#include <algorithm>
#include <span>
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

        m_resources = {};
        m_instance_buffer_static = nullptr;
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
        m_transfer_buffer = nullptr;
        m_transfer_semaphore = nullptr;
        m_transfer_commands = nullptr;
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
        StagingBufferInfo const transfer_buffer_info {.size = BufferRingAllocator::default_chunk_size * 16 };
        m_transfer_buffer = std::make_unique<BufferRingAllocator>(m_device->AllocateBuffer(transfer_buffer_info), BufferRingAllocator::default_chunk_size);
        m_transfer_queue = m_device->CreateTransferQueue();
        m_transfer_commands = m_transfer_queue->CreateCommandBuffer();
        m_transfer_semaphore = m_device->CreateTimelineSemaphore();

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

        // ---------------------------- TEST ----------------------------

        CreateResourceFamily(ResourceFamilyName{0});

        {
            auto buffer = m_device->AllocateBuffer(VertexBufferInfo{.size = 2048});
            m_instance_buffer_static = std::make_unique<BufferLinearAllocator>(std::move(buffer));
        }

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

        // Test - transfer data do the vertex buffer
        {
            m_transfer_commands->Reset();
            m_transfer_commands->Begin();
 
            
            {
                auto vb_handle = AllocateVertexbuffer(ResourceFamilyName{0}, sizeof(Gfx::VertexBasic) * 8);

                UploadVertexBuffer(vb_handle, [](BufferWriter& writer){
                    // front-left-bottom
                    writer << Float3{-1, -1,  1};
                    writer << Float3{1, 0, 0};
                    // front-right-top
                    writer << Float3{1,  1,  1};
                    writer << Float3{1, 0, 0};
                    // front-left-top
                    writer << Float3{-1,  1,  1};
                    writer << Float3{1, 0, 0};
                    // front-right-bottom
                    writer << Float3{1, -1,  1};
                    writer << Float3{1, 0, 0};
                    // back-left-bottom
                    writer << Float3{-1, -1, -1};
                    writer << Float3{0, 1, 0};
                    // back-right-top
                    writer << Float3{1,  1, -1};
                    writer << Float3{0, 1, 0};
                    // back-left-top
                    writer << Float3{-1,  1, -1};
                    writer << Float3{0, 0, 1};
                    // back-right-bottom
                    writer << Float3{1, -1, -1};
                    writer << Float3{0, 0, 1};
                });
            }
            {
                auto ib_handle = AllocateIndexbuffer(ResourceFamilyName{0}, sizeof(uint16_t) * 36);

                UploadIndexBuffer(ib_handle, [](BufferWriter& writer){
                    std::array<uint16_t, 36> const data
                    {
                        // front
                        0, 1, 2, 0, 3, 1,
                        // back
                        4, 6, 5, 4, 5, 7,
                        // left
                        4, 2, 6, 4, 0, 2,
                        // right
                        3, 5, 1, 3, 7, 5,
                        // top
                        2, 1, 5, 2, 5, 6,
                        // bottom
                        4, 3, 0, 4, 7, 3
                    };

                    writer << data;
                });
            }

            // Instance data test
            {
                auto staging_region = m_transfer_buffer->Allocate(sizeof(float) * 4 * 4);
                // TODO: Throw when vb_region is nullopt? --------------------------------------------------------
                auto data = m_transfer_buffer->Map<std::byte>(*staging_region);

                auto dst_region = m_instance_buffer_static->Allocate(staging_region->Size());

                Gfx::Transformations tm;
                tm.yaw = Math::pi + (Math::pi / 4.0);
                auto t = tm.GetTransformations();
                t.StoreAs<float>(data);

                //data[0].transformations = tm.GetTransformations() Matrix4<float>::Identity();

                m_transfer_commands->TransferBuffer(*staging_region, dst_region);
            }

            m_transfer_commands->End();
            m_transfer_semaphore->Increment();
            m_transfer_queue->Submit(*m_transfer_commands, *m_transfer_semaphore);
            m_transfer_semaphore->Wait();
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

    VertexBufferHandle Renderer::AllocateVertexbuffer(ResourceFamilyName name, uint64_t size)
    {
        auto& resources = m_resources[std::to_underlying(name)];

        assert(resources.vertex_buffer_allocator != nullptr);

        auto const region = resources.vertex_buffer_allocator->Allocate(size);
        uint64_t const uid = resources.vertex_buffer_regions.size();
        resources.vertex_buffer_regions.push_back(region);

        return {name, uid};
    }

    IndexBufferHandle Renderer::AllocateIndexbuffer(ResourceFamilyName name, uint64_t size)
    {
        auto& resources = m_resources[std::to_underlying(name)];

        assert(resources.index_buffer_allocator != nullptr);

        auto const region = resources.index_buffer_allocator->Allocate(size);
        uint64_t const uid = resources.index_buffer_regions.size();
        resources.index_buffer_regions.push_back(region);

        return {name, uid};
    }

    void Renderer::UploadBuffer(BufferRegion region, std::function<void(BufferWriter&)>& writer_callback)
    {
        assert(writer_callback != nullptr);

        auto staging_region = m_transfer_buffer->Allocate(region.Size());
        // TODO: Throw when vb_region is nullopt? --------------------------------------------------------
        auto staging_data = m_transfer_buffer->Map<std::byte>(*staging_region);

        BufferWriter writer(staging_data);
        writer_callback(writer);

        // -------- transfer command

        m_transfer_commands->TransferBuffer(*staging_region, region);
    }

    void Renderer::UploadVertexBuffer(VertexBufferHandle handle, std::function<void(BufferWriter&)> writer_callback)
    {
        auto const family = std::to_underlying(handle.m_family);
        auto const index = handle.m_index;

        assert(m_resources[family].vertex_buffer_regions.size() >= index);
        // TODO: family assert

        UploadBuffer(m_resources[family].vertex_buffer_regions[index], writer_callback);
    }

    void Renderer::UploadIndexBuffer(IndexBufferHandle handle, std::function<void(BufferWriter&)> writer_callback)
    {
        auto const family = std::to_underlying(handle.m_family);
        auto const index = handle.m_index;

        assert(m_resources[family].index_buffer_regions.size() >= index);
        // TODO: family assert

        UploadBuffer(m_resources[family].index_buffer_regions[index], writer_callback);
    }

    void Renderer::BeginFrame()
    {
        m_frame = &m_frames[m_frame_number % m_frames.size()];

        m_frame->Begin();

        Test();
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

        {
            auto region = m_instance_buffer_static->GetBuffer().GetRegion();
            m_frame->commands->UseVertexBuffer(region);
        }
        {
            //AllocateVertexbuffer(ResourceFamilyName{0}, )
            auto region = m_resources[0].vertex_buffer_allocator->GetBuffer().GetRegion();
            m_frame->commands->UseVertexBuffer(region);
        }
        {
            auto region = m_resources[0].index_buffer_allocator->GetBuffer().GetRegion();
            m_frame->commands->UseIndexBuffer(region);
        }

        m_swap_chain->AcquireNextImage();

        m_frame->BeginTestRenderPass(
            *m_test_vertex_pipeline,
            m_swap_chain->GetRenderTargetView(),
            render_pass_context
        );

        m_frame->BindVertexBuffer(m_resources[0].vertex_buffer_allocator->GetBuffer(), 0, 0); // ---------------------- Use VertexBinding !!!!!!!!
        m_frame->BindVertexBuffer(m_instance_buffer_static->GetBuffer(), 1, 0);
        m_frame->BindIndexBuffer(m_resources[0].index_buffer_allocator->GetBuffer(), IndexType::Uint16, 0);

        m_frame->Draw(36, 1, 0, 0, 0);

        m_frame->EndRenderPass();
    }

} // Rc::Render