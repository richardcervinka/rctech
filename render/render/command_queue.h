#pragma once

#include "vulkan/device.h"
#include "command_buffer.h"
#include "fence.h"
#include "semaphore.h"

namespace Rc::Render
{
    class RenderCommandQueue
    {
    public:
        RenderCommandQueue(VulkanDevice const& vk_device, uint32_t queue_family_index, uint32_t queue_index);

        ~RenderCommandQueue() = default;

        RenderCommandQueue(RenderCommandQueue const&) = delete;
        RenderCommandQueue& operator=(RenderCommandQueue const&) = delete;
        RenderCommandQueue(RenderCommandQueue&& other) = delete;
        RenderCommandQueue& operator=(RenderCommandQueue&& other) = delete;

        std::unique_ptr<RenderCommandBuffer> CreateCommandBuffer();

        // Submit render commands and reset internal state.
        void Submit(RenderCommandBuffer const& cb, Fence const& fence);

        uint32_t FamilyIndex() const
        {
            return m_vk_family;
        }

        VkQueue Handle() const
        {
            return m_vk_queue;
        }

        void WaitSemaphore(Semaphore const& semaphore)
        {
            m_vk_submit_wait = semaphore.Handle();
        }

        void SignalSemaphore(Semaphore const& semaphore)
        {
            m_vk_submit_signal = semaphore.Handle();
        }

    private:
        VulkanDevice const* m_vk_device {nullptr};
        uint32_t m_vk_family {};
        VkQueue m_vk_queue {VK_NULL_HANDLE};

        VkSemaphore m_vk_submit_wait {VK_NULL_HANDLE};
        VkSemaphore m_vk_submit_signal {VK_NULL_HANDLE};
    };

    class TransferCommandQueue
    {
    public:
        TransferCommandQueue(VulkanDevice const& vk_device, uint32_t queue_family_index, uint32_t queue_index);

        ~TransferCommandQueue() = default;

        TransferCommandQueue(TransferCommandQueue const&) = delete;
        TransferCommandQueue& operator=(TransferCommandQueue const&) = delete;
        TransferCommandQueue(TransferCommandQueue&& other) = delete;
        TransferCommandQueue& operator=(TransferCommandQueue&& other) = delete;

        std::unique_ptr<TransferCommandBuffer> CreateCommandBuffer();

        // Submit render commands and reset internal state.
        void Submit(TransferCommandBuffer const& cb, TimelineSemaphore& signal);

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