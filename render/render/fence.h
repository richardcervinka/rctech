#pragma once

#include "vulkan/device.h"

namespace Rc::Render
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

        VkFence Handle() const { return vk_fence; }

        void Wait() const;

        void Reset();

    private:
        //friend class CommandQueue;
        
        VulkanDevice const& vk_device;
        VkFence vk_fence {VK_NULL_HANDLE};
    };

} // Rc::Render