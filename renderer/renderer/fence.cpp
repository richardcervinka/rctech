#include "fence.h"

namespace Rc
{
    Fence::Fence(VulkanDevice const& vk_device) : m_vk_device{&vk_device}
    {
        VkFenceCreateInfo const create_info
        {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = VK_FENCE_CREATE_SIGNALED_BIT
        };

        m_vk_fence = m_vk_device->CreateFence(create_info);
    }

    Fence::~Fence()
    {
        if (m_vk_device != nullptr)
        {
            m_vk_device->DestroyFence(m_vk_fence);
        }
    }

    void Fence::Wait()
    {
        m_vk_device->WaitForFence(m_vk_fence);
        m_vk_device->ResetFence(m_vk_fence);
    }

} // Rc