#pragma once

#include "vulkan/device.h"
#include "command_buffer.h"
#include "fence.h"
#include "semaphore.h"

namespace Rc::Render
{
    class CommandQueue
    {
    public:
        CommandQueue(VulkanDevice const& vk_device, uint32_t queue_family_index, uint32_t queue_index);

        ~CommandQueue() = default;

        CommandQueue(CommandQueue const&) = delete;
        CommandQueue& operator=(CommandQueue const&) = delete;
        CommandQueue(CommandQueue&& other) = delete;
        CommandQueue& operator=(CommandQueue&& other) = delete;

        std::unique_ptr<CommandBuffer> CreateCommandBuffer();

        void WaitFor();

        // Submit render commands
        void Submit(
            CommandBuffer const& cb,
            Semaphore const& wait_semaphore,
            Semaphore const& signal_semaphore,
            Fence const& fence
        );

        uint32_t FamilyIndex() const
        {
            return m_vk_family;
        }

        VkQueue Handle() const
        {
            return m_vk_queue;
        }

    private:
        VulkanDevice const* m_vk_device {nullptr};
        uint32_t m_vk_family {};
        VkQueue m_vk_queue {VK_NULL_HANDLE};
    };

} // Rc::Render