#include "command_queue.h"
#include "error.h"

namespace Rc::Render
{
    // RenderCommandQueue

    RenderCommandQueue::RenderCommandQueue(VulkanDevice const& vk_device, uint32_t queue_family_index, uint32_t queue_index) :
        m_vk_device{&vk_device},
        m_vk_family{queue_family_index}
    {
        m_vk_queue = m_vk_device->GetDeviceQueue(queue_family_index, queue_index);
    }

    std::unique_ptr<RenderCommandBuffer> RenderCommandQueue::CreateCommandBuffer()
    {
        return std::make_unique<RenderCommandBuffer>(*m_vk_device, m_vk_family);
    }

    void RenderCommandQueue::Submit(RenderCommandBuffer const& cb, Fence const& fence)
    {
        VkPipelineStageFlags const stage_flags
        {
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        };

        auto command_buffers = cb.Handle();

        VkSubmitInfo submit_info
        {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext = nullptr,
            .waitSemaphoreCount = 0,
            .pWaitSemaphores = nullptr,
            .pWaitDstStageMask = &stage_flags,
            .commandBufferCount = 1,
            .pCommandBuffers = &command_buffers,
            .signalSemaphoreCount = 0,
            .pSignalSemaphores = nullptr
        };

        if (m_vk_submit_wait != VK_NULL_HANDLE)
        {
            submit_info.waitSemaphoreCount = 1u;
            submit_info.pWaitSemaphores = &m_vk_submit_wait;
        }
        if (m_vk_submit_signal != VK_NULL_HANDLE)
        {
            submit_info.signalSemaphoreCount = 1u;
            submit_info.pSignalSemaphores = &m_vk_submit_signal;
        }
        
        m_vk_device->QueueSubmit(m_vk_queue, submit_info, fence.Handle());

        m_vk_submit_wait = VK_NULL_HANDLE;
        m_vk_submit_signal =VK_NULL_HANDLE;
    }

    // TransferCommandQueue

    TransferCommandQueue::TransferCommandQueue(VulkanDevice const& vk_device, uint32_t queue_family_index, uint32_t queue_index) :
        m_vk_device{&vk_device},
        m_vk_family{queue_family_index}
    {
        m_vk_queue = m_vk_device->GetDeviceQueue(queue_family_index, queue_index);
    }

    std::unique_ptr<TransferCommandBuffer> TransferCommandQueue::CreateCommandBuffer()
    {
        return std::make_unique<TransferCommandBuffer>(*m_vk_device, m_vk_family);
    }

    void TransferCommandQueue::Submit(TransferCommandBuffer const& cb, TimelineSemaphore& signal)
    {
        auto command_buffers = cb.Handle();
        auto signal_semaphores = signal.Handle();
        auto value = signal.Increment();

        VkTimelineSemaphoreSubmitInfo timeline_info
        {
            .sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO,
            .pNext = NULL,
            .waitSemaphoreValueCount = 0,
            .pWaitSemaphoreValues = nullptr,
            .signalSemaphoreValueCount = 1,
            .pSignalSemaphoreValues = &value
        };

        VkSubmitInfo submit_info
        {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext = &timeline_info,
            .waitSemaphoreCount = 0,
            .pWaitSemaphores = nullptr,
            .pWaitDstStageMask = nullptr,
            .commandBufferCount = 1,
            .pCommandBuffers = &command_buffers,
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = &signal_semaphores
        };
        
        m_vk_device->QueueSubmit(m_vk_queue, submit_info, VK_NULL_HANDLE);
    }

} // Rc::Render