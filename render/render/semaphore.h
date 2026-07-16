#pragma once

#include "vulkan/device.h"
#include <chrono>

namespace Rc::Render
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

        VkSemaphore Handle() const
        {
            return vk_semaphore;
        }

    private:
        VulkanDevice const& vk_device;
        VkSemaphore vk_semaphore {VK_NULL_HANDLE};
    };

    class TimelineSemaphore
    {
    public:
        explicit TimelineSemaphore(VulkanDevice const& vk_device);
        
        ~TimelineSemaphore();

        TimelineSemaphore(TimelineSemaphore const&) = delete;
        TimelineSemaphore& operator=(TimelineSemaphore const&) = delete;
        TimelineSemaphore(TimelineSemaphore&& other) = delete;
        TimelineSemaphore& operator=(TimelineSemaphore&& other) = delete;

        void Wait(std::chrono::nanoseconds timeout = std::chrono::nanoseconds::max()) const
        {
            WaitFor(value, timeout);
        }

        void WaitFor(uint64_t value, std::chrono::nanoseconds timeout = std::chrono::nanoseconds::max()) const;

        uint64_t Increment()
        {
            return ++value;
        }

        void Set(uint64_t value)
        {
            this->value = value;
        }

        uint64_t Value() const
        {
            return value;
        }

        // Query the current value.
        uint64_t QueryCounter() const;

        VkSemaphore Handle() const
        {
            return vk_semaphore;
        }

    private:
        VulkanDevice const& vk_device;
        VkSemaphore vk_semaphore {VK_NULL_HANDLE};
        uint64_t value {0};
    };

} // Rc::Render