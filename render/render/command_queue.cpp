#include "command_queue.h"
#include "error.h"

namespace Rc::Render
{
    CommandQueue::CommandQueue(VulkanDevice const& vk_device, uint32_t queue_family_index, uint32_t queue_index) :
        m_vk_device{&vk_device},
        m_vk_family{queue_family_index}
    {
        m_vk_queue = m_vk_device->GetDeviceQueue(queue_family_index, queue_index);
    }

    std::unique_ptr<CommandBuffer> CommandQueue::CreateCommandBuffer()
    {
        return std::make_unique<CommandBuffer>(*m_vk_device, m_vk_family);
    }

    void CommandQueue::Submit(
        CommandBuffer const& cb,
        Semaphore const& wait_semaphore,
        Semaphore const& signal_semaphore,
        Fence const& fence)
    {
        auto vk_wait_semaphore = wait_semaphore.Handle();
        auto vk_signal_semaphore = signal_semaphore.Handle();

        VkPipelineStageFlags const stage_flags
        {
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        };

        VkSubmitInfo const submit_info
        {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext = nullptr,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &vk_wait_semaphore,
            .pWaitDstStageMask = &stage_flags,
            .commandBufferCount = 1,
            .pCommandBuffers = &(cb.m_vk_command_buffer),
            .signalSemaphoreCount =1,
            .pSignalSemaphores = &vk_signal_semaphore
        };
        
        m_vk_device->QueueSubmit(m_vk_queue, submit_info, fence.Handle());
    }

} // Rc::Render