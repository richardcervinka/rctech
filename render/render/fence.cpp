#include "fence.h"

namespace Rc::Render
{
    Fence::Fence(VulkanDevice const& vk_device) : vk_device{vk_device}
    {
        VkFenceCreateInfo const create_info
        {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = VK_FENCE_CREATE_SIGNALED_BIT
        };

        vk_fence = vk_device.CreateFence(create_info);
    }

    Fence::~Fence()
    {
        vk_device.DestroyFence(vk_fence);
    }

    void Fence::Wait() const
    {
        vk_device.WaitForFence(vk_fence);
    }

    void Fence::Reset()
    {
        vk_device.ResetFence(vk_fence);
    }

} // Rc::Render