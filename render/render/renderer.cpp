#include "renderer.h"
#include "index_buffer.h"
#include "resources.h"
#include <cassert>
#include <algorithm>
#include <span>
#include <iostream>
#include <stdexcept>
#include "platform/log.h"
#include "core/vertex.h"
#include "base/math.h"
#include "base/stopwatch.h"
#include "core/transformations.h"
#include "core/camera.h"
#include "generic/input.h"
#include "constants.h"
#include "development.h"
#include <thread>
#include <chrono>

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

        Rc::Dev::test_texture = nullptr;

        sampler_descriptor_heap = nullptr;
        resource_descriptor_heap = nullptr;
        resource_uploader = nullptr;
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

        render_fence = nullptr;
        render_commands = nullptr;
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
        swap_chain = device->CreateSwapChain(*surface, window, ColorProfile::SDR);
        render_queue = device->CreateGraphicsQueue();
        render_commands = render_queue->CreateCommandBuffer();
        render_fence = device->CreateFence();
        pipeline_layout = device->CreatePipelineLayout();
        resource_descriptor_heap = device->CreateResourceDescriptorHeap(32 * 1024);
        sampler_descriptor_heap = device->CreateSamplerDescriptorHeap(256);

        // TEST
        // ---------------------------- TEST ----------------------------
        Rc::Dev::test_texture = device->AllocateTexture2d(16, 16, PixelFormat::ColorRGBA);
        assert(Rc::Dev::test_texture != nullptr);
        resource_descriptor_heap->WriteTexture2dDescriptor(4, *Rc::Dev::test_texture);
        sampler_descriptor_heap->WriteDefaultSampler(0);

        // Create frames in flight.
        frames.resize(3);

        // Initialize frames in flight.
        for (std::size_t i = 0; i < frames.size(); i++)
        {
            auto& frame = frames[i];

            static constexpr uint64_t staging_buffer_size = 2048; // --------------------- docasne

            frame.uniform_buffer_index = static_cast<uint32_t>(i);
            //frame.queue = device->CreateGraphicsQueue();
            frame.commands = render_queue->CreateCommandBuffer();
            frame.fence = device->CreateFence();
            frame.staging_buffer = std::make_unique<BufferLinearAllocator>(device->AllocateStagingBuffer(staging_buffer_size));
            frame.instance_buffer = device->AllocateInstanceBuffer(512 * 512 * sizeof(Gfx::VertexInstance)); // ---------------------------------------------------------------- Size?
            frame.render_pass_uniform_buffer = device->AllocateUniformBuffer(1024);
            frame.depth_buffer = device->AllocateDepthBuffer(swap_chain->Width(), swap_chain->Height());
            frame.depth_buffer_view = frame.depth_buffer->CreateDepthBufferView();

            // Write resource descriptors...

            resource_descriptor_heap->WriteUniformBufferDescriptor(
                frame.uniform_buffer_index,
                frame.render_pass_uniform_buffer->Address(),
                frame.render_pass_uniform_buffer->Size()
            );
        }

        UploadResourceDescriptorHeap(*resource_descriptor_heap);
        UploadSamplerDescriptorHeap(*sampler_descriptor_heap);

        // Create embedded shaders.
        InitializeShaders();

        // Handle window resizing.
        window.OnEventSize(on_window_size);

        resource_manager = std::make_unique<ResourceManager>(*device);
        resource_uploader = std::make_unique<ResourceUploader>(*device, render_queue->FamilyIndex());

        resource_manager->ReserveVertexBuffer(ResourceFamily{0}, 2048 * 32);
        resource_manager->ReserveIndexBuffer(ResourceFamily{0}, 2048 * 32);

        {
            auto factory = device->CreatePipelineFactory();
            factory.SetPipelineLayout(pipeline_layout);
            factory.SetVertexShader(GetVertexShader(VertexShaderSlot::Overlay));
            factory.SetPixelShader(GetPixelShader(PixelShaderSlot::Null));
            factory.SetOutputFormat(swap_chain->Format());
            test_pipeline = factory.Create();
        }
        {
            auto factory = device->CreatePipelineFactory();
            factory.SetPipelineLayout(pipeline_layout);
            factory.SetVertexShader(GetVertexShader(VertexShaderSlot::Test));
            factory.SetPixelShader(GetPixelShader(PixelShaderSlot::Null));
            factory.SetVertexBinding(Gfx::VertexBinding::PerVertex, sizeof(Gfx::VertexBasic));
            factory.SetVertexBinding(Gfx::VertexBinding::PerInstance, sizeof(Gfx::VertexInstance));
            factory.SetOutputFormat(swap_chain->Format());
            factory.SetVertexAttributes({
                Gfx::VertexBasic::Attributes(),
                Gfx::VertexInstance::Attributes()
            });
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

        for (auto& frame : frames)
        {
            frame.Resize(*device, width, height);
        }
    }

    void Renderer::BeginFrame()
    {
        //std::this_thread::sleep_for(std::chrono::milliseconds{1});
        frame = &frames[frame_number % frames.size()];

        swap_chain->AcquireNextImage();

        resource_uploader->QueryCounter();

        frame->Begin();

        if (test_model != nullptr)
        {
            Test();
        }
    }

    void Renderer::EndFrame()
    {
        //frame->commands->BarierPresentSwapChain(swap_chain->GetRenderTargetView()); // ---------------
        frame->commands->RenderTargetBarier(
            swap_chain->GetRenderTargetView(),
            ImageUsage::ColorAttachmentWrite,
            ImageUsage::Present,
            ImageLayout::ColorAttachment,
            ImageLayout::Present,
            ImageAccess::ColorAttachmentWrite,
            ImageAccess::None
        );
        frame->commands->End();
        
        render_queue->WaitSemaphore(swap_chain->GetAcquireSemaphore());
        render_queue->SignalSemaphore(swap_chain->GetPresentSemaphore());
        render_queue->Submit(*frame->commands, *frame->fence);

        swap_chain->Present(*render_queue);

        frame_number += 1;

        // Submit copy commands.
        if (resource_uploader->PendingTransfer())
        {
            auto lock = std::unique_lock(*resource_uploader, std::defer_lock);

            if (lock.try_lock())
            {
                resource_uploader->Transfer();
            }
        }

        
    }

    static Gfx::PerspectiveCamera CreateTestCamera()
    {
        Gfx::PerspectiveCamera camera;
        camera.transformations.z = 3.0; // TODO: ------------ world_to_view=translate(−cameraPosition)
        //camera.transformations.yaw = Math::pi;
        camera.fov = Math::DegToRad(75.0);
        return camera;
    }

    void Renderer::Test()
    {
        static Gfx::PerspectiveCamera camera = CreateTestCamera();
        
        auto camera_rotations = camera.transformations.GetRotations();
        auto camera_matrix = camera_rotations.ToMatrix<double>();

        Vector4<double> camera_right
        {
            camera_matrix.At(0, 0),
            camera_matrix.At(1, 0),
            camera_matrix.At(2, 0),
            camera_matrix.At(3, 0)
        };

        Vector4<double> camera_up
        {
            camera_matrix.At(0, 1),
            camera_matrix.At(1, 1),
            camera_matrix.At(2, 1),
            camera_matrix.At(3, 1)
        };

        Vector4<double> camera_forward
        {
            camera_matrix.At(0, 2),
            camera_matrix.At(1, 2),
            camera_matrix.At(2, 2),
            camera_matrix.At(3, 2)
        };

        camera_forward *= 0.02;
        camera_right *= 0.02;
        camera_up *= 0.02;
        // t.AppendTranslation()

        if (Input::Pushed(Input::KeyCode::A))
        {
            camera.transformations.x -= camera_right.x;
            camera.transformations.y -= camera_right.y;
            camera.transformations.z -= camera_right.z;
        }
        if (Input::Pushed(Input::KeyCode::D))
        {
            camera.transformations.x += camera_right.x;
            camera.transformations.y += camera_right.y;
            camera.transformations.z += camera_right.z;
        }
        if (Input::Pushed(Input::KeyCode::W))
        {
            camera.transformations.x -= camera_forward.x;
            camera.transformations.y -= camera_forward.y;
            camera.transformations.z -= camera_forward.z;
        }
        if (Input::Pushed(Input::KeyCode::S))
        {
            camera.transformations.x += camera_forward.x;
            camera.transformations.y += camera_forward.y;
            camera.transformations.z += camera_forward.z;
        }
        if (Input::Pushed(Input::KeyCode::RightArrow))
        {
            camera.transformations.yaw -= 0.01; 
        }
        if (Input::Pushed(Input::KeyCode::LeftArrow))
        {
            camera.transformations.yaw += 0.01; 
        }
        if (Input::Pushed(Input::KeyCode::UpArrow))
        {
            camera.transformations.pitch += 0.01; 
        }
        if (Input::Pushed(Input::KeyCode::DownArrow))
        {
            camera.transformations.pitch -= 0.01; 
        }
        if (Input::Pushed(Input::KeyCode::Spacebar))
        {
            camera.transformations.y += 0.02;
        }
        if (Input::Pushed(Input::KeyCode::LeftControl))
        {
            camera.transformations.y -= 0.02;
        }

        Stopwatch stopwatch;

        RenderPassContext render_pass_context {};
        render_pass_context.camera = &camera;

        // Build instance buffer.
        for (auto& instnace : test_model->instances)
        {
            frame->WriteInstance({
                .local_transformations = instnace.Local().To<float>(),
                .world_transformations = instnace.World().To<float>()
            });
        }

        // TODO: Toto je jen docasne reseni
        static bool barier = true;
        if (barier)
        {
            frame->commands->MemoryBarier(
                resource_manager->GetBufferRegion(test_model->vb_handle),
                BufferUsage::Undefined,
                BufferUsage::VertexBuffer
            );
            frame->commands->MemoryBarier(
                resource_manager->GetBufferRegion(test_model->ib_handle),
                BufferUsage::Undefined,
                BufferUsage::IndexBuffer
            );

            barier = false;
        }

        frame->BeginTestRenderPass(
            *test_vertex_pipeline,
            *resource_descriptor_heap,
            *sampler_descriptor_heap,
            swap_chain->GetRenderTargetView(),
            render_pass_context
        );

        frame->BindVertexBuffer(resource_manager->GetVertexBuffer(ResourceFamily{0}), 0); // ---------------------- Use VertexBinding !!!!!!!!
        frame->BindInstanceBuffer(0);
        frame->BindIndexBuffer(resource_manager->GetIndexBuffer(ResourceFamily{0}), IndexType::Uint16, 0);

        frame->Draw(36, 420 * 420, 0, 0, 0);

        frame->EndRenderPass();

        // const auto render_time = stopwatch.Elapsed<std::chrono::milliseconds>();
        // return;
    }

    void Renderer::UploadResourceDescriptorHeap(ResourceDescriptorHeap& descriptor_heap)
    {
        auto const data = descriptor_heap.Data();
        auto buffer = descriptor_heap.GetBufferRegion();

        auto transfer_buffer = device->AllocateStagingBuffer(data.size());
        auto const staging_region = transfer_buffer->GetRegion(0, data.size());
        // TODO: Throw when vb_region is nullopt? --------------------------------------------------------

        // Write data heap to the transfer buffer.
        auto const memory = transfer_buffer->Map(staging_region);
        std::copy(data.begin(), data.end(), memory.begin());

        render_fence->Wait();

        // Transfer the staging buffer.
        render_commands->Reset();
        render_commands->Begin();
        render_commands->MemoryBarier(buffer, BufferUsage::Undefined, BufferUsage::TransferWrite);
        render_commands->TransferBuffer(staging_region, buffer);
        render_commands->MemoryBarier(buffer, BufferUsage::TransferWrite, BufferUsage::ResourceDescriptorHeap);
        render_commands->End();

        // Submit commands.
        //auto fence = device->CreateFence(); // ---------------- Sdilene, stejne jako render_commands
        render_fence->Reset();
        render_queue->Submit(*render_commands, *render_fence);
        render_fence->Wait();
    }

    void Renderer::UploadSamplerDescriptorHeap(SamplerDescriptorHeap& descriptor_heap)
    {
        auto const data = descriptor_heap.Data();
        auto buffer = descriptor_heap.GetBufferRegion();

        auto transfer_buffer = device->AllocateStagingBuffer(data.size());
        auto const staging_region = transfer_buffer->GetRegion(0, data.size());
        // TODO: Throw when vb_region is nullopt? --------------------------------------------------------

        // Write data heap to the transfer buffer.
        auto const memory = transfer_buffer->Map(staging_region);
        std::copy(data.begin(), data.end(), memory.data());

        render_fence->Wait();

        // Transfer the staging buffer.
        render_commands->Reset();
        render_commands->Begin();
        render_commands->MemoryBarier(buffer, BufferUsage::Undefined, BufferUsage::TransferWrite);
        render_commands->TransferBuffer(staging_region, buffer);
        render_commands->MemoryBarier(buffer, BufferUsage::TransferWrite, BufferUsage::SamplerDescriptorHeap);
        render_commands->End();

        // Submit commands.
        //auto fence = device->CreateFence(); // ---------------- Sdilene, stejne jako render_commands
        render_fence->Reset();
        render_queue->Submit(*render_commands, *render_fence);
        render_fence->Wait();
    }

} // Rc::Render