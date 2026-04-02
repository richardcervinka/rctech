#pragma once

#include "vulkan/device.h"

namespace Rc
{
    //
    // GPU-CPU synchronization
    //
    class Semaphore
    {
    public:
        explicit Semaphore(VulkanDevice const& vk_device);
        
        ~Semaphore();

        Semaphore(Semaphore const&) = delete;
        Semaphore& operator=(Semaphore const&) = delete;
        Semaphore(Semaphore&& other) = delete;
        Semaphore& operator=(Semaphore&& other) = delete;

        VkSemaphore Handle() const { return m_vk_semaphore; }

    private:
        friend class CommandQueue;
        
        VulkanDevice const* m_vk_device {nullptr};
        VkSemaphore m_vk_semaphore {VK_NULL_HANDLE};
    };

} // Rc