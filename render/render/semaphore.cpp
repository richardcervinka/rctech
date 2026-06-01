#include "semaphore.h"

namespace Rc::Render
{
    Semaphore::Semaphore(VulkanDevice const& vk_device) : m_vk_device{&vk_device}
    {
        VkSemaphoreCreateInfo const create_info
        {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0
        };

        m_vk_semaphore = m_vk_device->CreateSemaphore(create_info);
    }

    Semaphore::~Semaphore()
    {
        if (m_vk_device != nullptr)
        {
            m_vk_device->DestroySemaphore(m_vk_semaphore);
        }
    }

} // Rc::Render