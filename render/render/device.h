#pragma once

#include "vulkan/instance.h"
#include "vulkan/device.h"
#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <cstddef>
#include <map>
#include "swap_chain.h"
#include "command_queue.h"
#include "pipeline_state.h"
#include "fence.h"
#include "platform/window.h"
#include "shader.h"
#include "surface.h"
#include "buffer.h"
#include "descriptor.h"

namespace Rc::Render
{
    struct QueueFamilyProperties
    {
        uint32_t index;
        uint32_t count;
        bool graphics;
        bool transfer;
        bool compute;
        bool presentation;
        bool surface;
    };

    class Device
    {
    public:
        Device(
            VulkanContext const& context,
            VulkanInstance const& instance,
            VkPhysicalDevice vk_physical_device, 
            Surface const& surface
        );

        Device(Device const&) = delete;
        Device& operator=(Device const&) = delete;
        Device(Device&& other) = delete;
        Device& operator=(Device&& other) = delete;

        ~Device();

        std::unique_ptr<SwapChain> CreateSwapChain(Surface const& surface, Window const& window);

        std::unique_ptr<Shader> CreateShader(std::span<uint32_t const> spirv);

        std::unique_ptr<RenderCommandQueue> CreateGraphicsQueue();
        std::unique_ptr<TransferCommandQueue> CreateTransferQueue();

        std::unique_ptr<Fence> CreateFence();
        std::unique_ptr<Semaphore> CreateSemaphore() const;
        std::unique_ptr<TimelineSemaphore> CreateTimelineSemaphore() const;

        void WaitIdle() const noexcept;

        std::shared_ptr<PipelineLayout> CreatePipelineLayout();

        PipelineFactory CreatePipelineFactory();

        std::unique_ptr<ResourceDescriptorHeap> CreateResourceDescriptorHeap();

        std::unique_ptr<Buffer> AllocateVertexBuffer(uint64_t size) const;
        std::unique_ptr<Buffer> AllocateIndexBuffer(uint64_t size) const;
        std::unique_ptr<Buffer> AllocateStagingBuffer(uint64_t size) const;
        std::unique_ptr<Buffer> AllocateUniformBuffer(uint64_t size) const;
        std::unique_ptr<Buffer> AllocateDescriptorHeapBuffer(uint64_t size) const;

    private:
        std::map<std::string, VulkanVersion> EnumerateExtensions() const;
        std::vector<QueueFamilyProperties> GetQueueFamilyProperties(Surface const& surface) const;

        VulkanInstance const* m_instance {nullptr}; //------------------------- ref ?
        std::unique_ptr<VulkanDevice> m_device;
        
        VkPhysicalDevice m_vk_physical_device {VK_NULL_HANDLE};

        // Allocator m_allocator;

        VmaAllocator m_vma_allocator {VK_NULL_HANDLE};

        //Queue m_render_queue;

        // Rendering queue info <family index, queue index>
        std::pair<uint32_t, uint32_t> m_vk_graphics_queue_family;

        // Transfer queue info <family index, queue index>
        std::pair<uint32_t, uint32_t> m_vk_transfer_queue_family;
    };

} // Rc::Render