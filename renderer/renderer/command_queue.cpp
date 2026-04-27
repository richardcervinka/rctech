#include "command_queue.h"
#include "error.h"

namespace Rc
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

    void CommandQueue::Submit(CommandBuffer const& cb, Fence const& fence)
    {
        VkSubmitInfo const submit_info
        {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext = nullptr,
            .waitSemaphoreCount = 0,
            .pWaitSemaphores = nullptr,
            .pWaitDstStageMask = nullptr,
            .commandBufferCount = 1,
            .pCommandBuffers = &(cb.m_vk_buffer),
            .signalSemaphoreCount = 0,
            .pSignalSemaphores = nullptr
        };
        
        m_vk_device->QueueSubmit(m_vk_queue, submit_info, fence.m_vk_fence);

        // vk::PipelineStageFlags wait_stages = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        // submit.waitSemaphoreCount = 0;
        // submit.pWaitSemaphores = //&(*vk_wait_semaphore);
        // submit.signalSemaphoreCount = 1;
        // submit.pSignalSemaphores = &(*vk_signal_semaphore);
        // submit.pWaitDstStageMask = &wait_stages;
    }

    void CommandQueue::Present(SwapChain const& sc) const
    {
        sc.Present(m_vk_queue);
    }

} // Rc