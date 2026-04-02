#pragma once

#include "vulkan/device.h"

namespace Rc
{
    //
    // GPU-CPU synchronization
    //
    class Fence
    {
    public:
        explicit Fence(VulkanDevice const& vk_device);

        ~Fence();

        Fence(Fence const&) = delete;
        Fence& operator=(Fence const&) = delete;
        Fence(Fence&& other) = delete;
        Fence& operator=(Fence&& other) = delete;

        VkFence Handle() const { return m_vk_fence; }

        void Wait();

    private:
        friend class CommandQueue;
        
        VulkanDevice const* m_vk_device {nullptr};
        VkFence m_vk_fence {VK_NULL_HANDLE};
    };

} // Rc