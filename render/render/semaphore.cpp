#include "semaphore.h"

namespace Rc::Render
{
    // Semaphore

    Semaphore::Semaphore(VulkanDevice const& vk_device) : vk_device{vk_device}
    {
        VkSemaphoreCreateInfo const create_info
        {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0
        };

        vk_semaphore = vk_device.CreateSemaphore(create_info);
    }

    Semaphore::~Semaphore()
    {
        vk_device.DestroySemaphore(vk_semaphore);
    }

    // TimelineSemaphore

    TimelineSemaphore::TimelineSemaphore(VulkanDevice const& vk_device) : vk_device{vk_device}
    {
        VkSemaphoreTypeCreateInfo const type_create_info
        {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
            .pNext = nullptr,
            .semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE,
            .initialValue = 0
        };

        VkSemaphoreCreateInfo const create_info
        {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            .pNext = &type_create_info,
            .flags = 0
        };

        vk_semaphore = vk_device.CreateSemaphore(create_info);
    }

    TimelineSemaphore::~TimelineSemaphore()
    {
        vk_device.DestroySemaphore(vk_semaphore);
    }

    void TimelineSemaphore::WaitFor(uint64_t value, std::chrono::nanoseconds timeout) const
    {
        VkSemaphoreWaitInfo const wait_info
        {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
            .pNext = NULL,
            .flags = 0,
            .semaphoreCount = 1,
            .pSemaphores = &vk_semaphore,
            .pValues = &value
        };

        vk_device.WaitSemaphores(wait_info, timeout);
    }

    uint64_t TimelineSemaphore::QueryCounter() const
    {
        return vk_device.GetSemaphoreCounterValue(vk_semaphore);
    }

} // Rc::Render