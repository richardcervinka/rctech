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
#include "base/image.h"

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
        transfer_queue = nullptr;
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
                
                return adapter->CreateDevice(*surface);
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
        transfer_queue = device->CreateTransferQueue();
        render_commands = render_queue->CreateCommandBuffer();
        render_fence = device->CreateFence();
        pipeline_layout = device->CreatePipelineLayout();
        resource_descriptor_heap = device->CreateResourceDescriptorHeap(32 * 1024);
        sampler_descriptor_heap = device->CreateSamplerDescriptorHeap(256);

        // ---------------------------- TEST ----------------------------
        Rc::Dev::test_texture = device->AllocateTexture2d(256, 256, false, PixelFormat::ColorSRGBA);
        assert(Rc::Dev::test_texture != nullptr);

        std::array<TextureLayout, 1> layout
        {
            TextureLayout
            {
                .width = 256,
                .height = 256,
                .mip_level = 0,
                .array_level = 0,
                .offset = 0,
                .size = 256 * 256 * 4
            }
        };
        
        auto const image = Image::Load("C:\\Users\\richa\\Pictures\\test.png");

        CopyTexture2d(image.Raw(), layout, *Rc::Dev::test_texture);



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

        CopyBuffer(
            resource_descriptor_heap->Data(),
            resource_descriptor_heap->GetBufferRegion(),
            BufferUsage::ResourceDescriptorHeap
        );

        CopyBuffer(
            sampler_descriptor_heap->Data(),
            sampler_descriptor_heap->GetBufferRegion(),
            BufferUsage::SamplerDescriptorHeap
        );

        // Create embedded shaders.
        InitializeShaders();

        resource_manager = std::make_unique<ResourceManager>(*device);
        resource_uploader = std::make_unique<ResourceUploader>(*device);

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

        // Handle window resizing.
        window.OnEventSize(on_window_size);

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

        frame->Begin(swap_chain->GetRenderTargetView());
        frame->BindResourceDescriptorHeap(*resource_descriptor_heap);
        frame->BindSamplerDescriptorHeap(*sampler_descriptor_heap);

        if (Rc::Dev::initialized)
        {
            Test();
        }
    }

    void Renderer::EndFrame()
    {
        frame->End(swap_chain->GetRenderTargetView());
        
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
        camera.transformations.y = 3.0;
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
        for (auto& instnace : Rc::Dev::instances)
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
            frame->commands->MemoryBarrier(
                resource_manager->GetBufferRegion(Rc::Dev::vb_handle),
                BufferUsage::Undefined,
                BufferUsage::VertexBuffer
            );
            frame->commands->MemoryBarrier(
                resource_manager->GetBufferRegion(Rc::Dev::ib_handle),
                BufferUsage::Undefined,
                BufferUsage::IndexBuffer
            );
            // frame->commands->BarrierTexture2dAcquire(
            //     *Rc::Dev::test_texture,
            //     ImageUsage::SampledImage,
            //     ImageUsage::SampledImage,
            //     transfer_queue->FamilyIndex(),
            //     render_queue->FamilyIndex()
            // );

            barier = false;
        }

        frame->BeginTestRenderPass(
            *test_vertex_pipeline,
            swap_chain->GetRenderTargetView(),
            render_pass_context
        );

        frame->BindVertexBuffer(resource_manager->GetVertexBuffer(ResourceFamily{0}), 0); // ---------------------- Use VertexBinding !!!!!!!!
        frame->BindInstanceBuffer(0);
        frame->BindIndexBuffer(resource_manager->GetIndexBuffer(ResourceFamily{0}), IndexType::Uint16, 0);

        frame->Draw(36, 420 * 420, 0, 0, 0);

        frame->EndRenderPass();
    }

    void Renderer::CopyBuffer(std::span<std::byte const> src, BufferRegion dst, BufferUsage usage)
    {
        auto buffer = device->AllocateStagingBuffer(src.size());
        auto region = buffer->GetRegion(0, src.size()); // TODO: Throw when vb_region is nullopt? --------------------------------------------------------
        auto memory = buffer->Map(region);

        // Write data heap to the transfer buffer.
        std::copy(src.begin(), src.end(), memory.data());

        // Transfer the staging buffer.
        render_commands->Reset();
        render_commands->Begin();
        render_commands->MemoryBarrier(dst, BufferUsage::Undefined, BufferUsage::TransferWrite);
        render_commands->TransferBuffer(region, dst);
        render_commands->MemoryBarrier(dst, BufferUsage::TransferWrite, usage);
        render_commands->End();

        // Submit commands.
        render_fence->Wait();
        render_fence->Reset();
        render_queue->Submit(*render_commands, *render_fence);
        render_fence->Wait();
    }

    void Renderer::CopyTexture2d(std::span<std::byte const> src, std::span<TextureLayout const> layout, Texture2d& dst)
    {
        auto buffer = device->AllocateStagingBuffer(src.size());
        auto region = buffer->GetRegion();  // TODO: Throw when vb_region is nullopt? Or Fallback --------------------------------------------------------
        auto memory = buffer->Map(region);

        std::copy(src.begin(), src.end(), memory.data());
        
        // Transfer the staging buffer.
        render_commands->Reset();
        render_commands->Begin();
        render_commands->Texture2dBarrier(dst, ImageUsage::Undefined, ImageUsage::TransferWrite);
        render_commands->TransferTexture(region, layout, dst);
        render_commands->Texture2dBarrier(dst, ImageUsage::TransferWrite, ImageUsage::SampledImage);
        render_commands->End();

        // Submit commands.
        render_fence->Wait();
        render_fence->Reset();
        render_queue->Submit(*render_commands, *render_fence);
        render_fence->Wait();
    }

} // Rc::Render