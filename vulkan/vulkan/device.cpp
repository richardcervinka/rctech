#include "device.h"
#include <cassert>

namespace Rc
{
    VulkanDevice::VulkanDevice(VulkanContext const& context, VkDevice vk_device) :
        m_context{context},
        m_vk_device{vk_device}
    {
        Load("vkDestroyDevice", m_vkDestroyDevice);
        Load("vkDeviceWaitIdle", m_vkDeviceWaitIdle);
        Load("vkCreateGraphicsPipelines", m_vkCreateGraphicsPipelines);
        Load("vkDestroyPipeline", m_vkDestroyPipeline);
        Load("vkCreateShaderModule", m_vkCreateShaderModule);
        Load("vkDestroyShaderModule", m_vkDestroyShaderModule);
        Load("vkCreateFence", m_vkCreateFence);
        Load("vkDestroyFence", m_vkDestroyFence);
        Load("vkCreateSemaphore", m_vkCreateSemaphore);
        Load("vkDestroySemaphore", m_vkDestroySemaphore);
        Load("vkWaitForFences", m_vkWaitForFences);
        Load("vkResetFences", m_vkResetFences);
        Load("vkCreateCommandPool", m_vkCreateCommandPool);
        Load("vkDestroyCommandPool", m_vkDestroyCommandPool);
        Load("vkAllocateCommandBuffers", m_vkAllocateCommandBuffers);
        Load("vkFreeCommandBuffers", m_vkFreeCommandBuffers);
        Load("vkResetCommandPool", m_vkResetCommandPool);
        Load("vkCreateSwapchainKHR", m_vkCreateSwapchainKHR);
        Load("vkDestroySwapchainKHR", m_vkDestroySwapchainKHR);
        Load("vkGetSwapchainImagesKHR", m_vkGetSwapchainImagesKHR);
        Load("vkAcquireNextImageKHR", m_vkAcquireNextImageKHR);
        Load("vkCreateImageView", m_vkCreateImageView);
        Load("vkDestroyImageView", m_vkDestroyImageView);
        Load("vkGetDeviceQueue", m_vkGetDeviceQueue);
        Load("vkCreatePipelineLayout", m_vkCreatePipelineLayout);
        Load("vkDestroyPipelineLayout", m_vkDestroyPipelineLayout);
        Load("vkQueueSubmit", m_vkQueueSubmit);
        Load("vkQueuePresentKHR", m_vkQueuePresentKHR);
        Load("vkBeginCommandBuffer", m_vkBeginCommandBuffer);
        Load("vkEndCommandBuffer", m_vkEndCommandBuffer);
        Load("vkCmdPipelineBarrier", m_vkCmdPipelineBarrier);
        Load("vkCmdBeginRendering", m_vkCmdBeginRendering);
        Load("vkCmdEndRendering", m_vkCmdEndRendering);
        Load("vkCmdDraw", m_vkCmdDraw);
        Load("vkCmdBindPipeline", m_vkCmdBindPipeline);
        Load("vkCmdSetViewport", m_vkCmdSetViewport);
        Load("vkCmdSetScissor", m_vkCmdSetScissor);
        Load("vkCmdCopyBuffer", m_vkCmdCopyBuffer);
        Load("vkCmdBindVertexBuffers", m_vkCmdBindVertexBuffers);
    }

    VulkanDevice::~VulkanDevice()
    {
        m_vkDestroyDevice(m_vk_device, nullptr);
    }

    void VulkanDevice::WaitIdle() const
    {
        if (auto vk_result = m_vkDeviceWaitIdle(m_vk_device); vk_result != VK_SUCCESS)
        {
            throw VulkanException(vk_result);
        }
    }

    VkPipeline VulkanDevice::CreateGraphicsPipeline(VkPipelineCache pipeline_cache, VkGraphicsPipelineCreateInfo const& create_info) const
    {
        VkPipeline result = VK_NULL_HANDLE;
        if (auto vk_result = m_vkCreateGraphicsPipelines(m_vk_device, pipeline_cache, 1, &create_info, nullptr, &result); vk_result != VK_SUCCESS)
        {
            throw VulkanException(vk_result);
        }
        return result;
    }

    void VulkanDevice::DestroyPipeline(VkPipeline& pipeline) const noexcept
    {
        m_vkDestroyPipeline(m_vk_device, pipeline, nullptr);
        pipeline = VK_NULL_HANDLE;
    }

    VkQueue VulkanDevice::GetDeviceQueue(uint32_t queue_family_index, uint32_t queue_index) const
    {
        VkQueue result = VK_NULL_HANDLE;
        m_vkGetDeviceQueue(m_vk_device, queue_family_index, queue_index, &result);
        return result;
    }

    VkPipelineLayout VulkanDevice::CreatePipelineLayout(VkPipelineLayoutCreateInfo const& create_info) const
    {
        VkPipelineLayout result = VK_NULL_HANDLE;
        if (auto vk_result = m_vkCreatePipelineLayout(m_vk_device, &create_info, nullptr, &result); vk_result != VK_SUCCESS)
        {
            throw VulkanException(vk_result);
        }
        return result;
    }

    void VulkanDevice::DestroyPipelineLayout(VkPipelineLayout pipeline_layout) const noexcept
    {
        m_vkDestroyPipelineLayout(m_vk_device, pipeline_layout, nullptr);
        pipeline_layout = VK_NULL_HANDLE;
    }

    VkShaderModule VulkanDevice::CreateShaderModule(VkShaderModuleCreateInfo const& create_info) const
    {
        VkShaderModule result = VK_NULL_HANDLE;
        if (auto vk_result = m_vkCreateShaderModule(m_vk_device, &create_info, nullptr, &result); vk_result != VK_SUCCESS)
        {
            throw VulkanException(vk_result);
        }
        return result;
    }

    void VulkanDevice::DestroyShaderModule(VkShaderModule& shader_module) const noexcept
    {
        m_vkDestroyShaderModule(m_vk_device, shader_module, nullptr);
        shader_module = VK_NULL_HANDLE;
    }

    VkFence VulkanDevice::CreateFence(VkFenceCreateInfo const& create_info) const
    {
        VkFence result = VK_NULL_HANDLE;
        if (auto vk_result = m_vkCreateFence(m_vk_device, &create_info, nullptr, &result); vk_result != VK_SUCCESS)
        {
            throw VulkanException(vk_result);
        }
        return result;
    }

    void VulkanDevice::DestroyFence(VkFence& fence) const noexcept
    {
        m_vkDestroyFence(m_vk_device, fence, nullptr);
        fence = VK_NULL_HANDLE;
    }

    VkSemaphore VulkanDevice::CreateSemaphore(VkSemaphoreCreateInfo const& create_info) const
    {
        VkSemaphore result = VK_NULL_HANDLE;
        if (auto vk_result = m_vkCreateSemaphore(m_vk_device, &create_info, nullptr, &result); vk_result != VK_SUCCESS)
        {
            throw VulkanException(vk_result);
        }
        return result;
    }

    void VulkanDevice::DestroySemaphore(VkSemaphore& semaphore) const noexcept
    {
        m_vkDestroySemaphore(m_vk_device, semaphore, nullptr);
        semaphore = VK_NULL_HANDLE;
    }

    void VulkanDevice::WaitForFences(std::span<VkFence const> fences, bool wait_all, uint64_t timeout) const
    {
        if (auto vk_result = m_vkWaitForFences(m_vk_device, fences.size(), fences.data(), VkBool32{wait_all}, timeout); vk_result != VK_SUCCESS)
        {
            throw VulkanException(vk_result);
        }
    }

    void VulkanDevice::WaitForFence(VkFence const& fence, uint64_t timeout) const
    {
        if (auto vk_result = m_vkWaitForFences(m_vk_device, 1, &fence, VK_TRUE, timeout); vk_result != VK_SUCCESS)
        {
            throw VulkanException(vk_result);
        }
    }

    void VulkanDevice::ResetFences(std::span<VkFence> fences) const
    {
        if (auto vk_result = m_vkResetFences(m_vk_device, fences.size(), fences.data()); vk_result != VK_SUCCESS)
        {
            throw VulkanException(vk_result);
        }
    }

    void VulkanDevice::ResetFence(VkFence& fence) const
    {
        if (auto vk_result = m_vkResetFences(m_vk_device, 1, &fence); vk_result != VK_SUCCESS)
        {
            throw VulkanException(vk_result);
        }
    }

    uint32_t VulkanDevice::AcquireNextImageKHR(VkSwapchainKHR swapchain, uint64_t timeout, VkSemaphore semaphore, VkFence fence) const
    {
        uint32_t result {};
        if (auto vk_result = m_vkAcquireNextImageKHR(m_vk_device, swapchain, timeout, semaphore, fence, &result); vk_result != VK_SUCCESS)
        {
            throw VulkanException(vk_result);
        }
        return result;
    }

    VkSwapchainKHR VulkanDevice::CreateSwapchainKHR(VkSwapchainCreateInfoKHR const& create_info) const
    {
        VkSwapchainKHR result = VK_NULL_HANDLE;
        if (auto vk_result = m_vkCreateSwapchainKHR(m_vk_device, &create_info, nullptr, &result); vk_result != VK_SUCCESS)
        {
            throw VulkanException(vk_result);
        }
        return result;
    }

    void VulkanDevice::DestroySwapchainKHR(VkSwapchainKHR swapchain) const noexcept
    {
        m_vkDestroySwapchainKHR(m_vk_device, swapchain, nullptr);
        swapchain = VK_NULL_HANDLE;
    }

    uint32_t VulkanDevice::GetSwapchainImagesKHRCount(VkSwapchainKHR swapchain) const
    {
        uint32_t result {};
        if (auto vk_result = m_vkGetSwapchainImagesKHR(m_vk_device, swapchain, &result, nullptr); vk_result != VK_SUCCESS)
        {
            throw VulkanException(vk_result);
        }
        return result;
    }

    std::span<VkImage> VulkanDevice::GetSwapchainImagesKHR(VkSwapchainKHR swapchain, std::span<VkImage> buffer) const
    {
        auto count = static_cast<uint32_t>(buffer.size());

        if (auto vk_result = m_vkGetSwapchainImagesKHR(m_vk_device, swapchain, &count, buffer.data()); vk_result != VK_SUCCESS)
        {
            throw VulkanException(vk_result);
        }
        return buffer.subspan(0, count);
    }

    VkCommandPool VulkanDevice::CreateCommandPool(VkCommandPoolCreateInfo const& create_info) const
    {
        VkCommandPool result = VK_NULL_HANDLE;
        if (auto vk_result = m_vkCreateCommandPool(m_vk_device, &create_info, nullptr, &result); vk_result != VK_SUCCESS)
        {
            throw VulkanException(vk_result);
        }
        return result;
    }

    void VulkanDevice::DestroyCommandPool(VkCommandPool& command_pool) const noexcept
    {
        m_vkDestroyCommandPool(m_vk_device, command_pool, nullptr);
        command_pool = VK_NULL_HANDLE;
    }

    VkCommandBuffer VulkanDevice::AllocateCommandBuffer(VkCommandBufferAllocateInfo const& allocate_info) const
    {
        assert(allocate_info.commandBufferCount == 1);

        VkCommandBuffer result = VK_NULL_HANDLE;
        if (auto vk_result = m_vkAllocateCommandBuffers(m_vk_device, &allocate_info, &result); vk_result != VK_SUCCESS)
        {
            throw VulkanException(vk_result);
        }
        return result;
    }

    void VulkanDevice::FreeCommandBuffer(VkCommandPool command_pool, VkCommandBuffer& command_buffer) const noexcept
    {
        m_vkFreeCommandBuffers(m_vk_device, command_pool, 1, &command_buffer);
        command_buffer = VK_NULL_HANDLE;
    }

    void VulkanDevice::ResetCommandPool(VkCommandPool command_pool, VkCommandPoolResetFlags flags) const
    {
        if (auto vk_result = m_vkResetCommandPool(m_vk_device, command_pool, flags); vk_result != VK_SUCCESS)
        {
            throw VulkanException(vk_result);
        }
    }

    VkImageView VulkanDevice::CreateImageView(VkImageViewCreateInfo const& create_info) const
    {
        VkImageView result = VK_NULL_HANDLE;
        if (auto vk_result = m_vkCreateImageView(m_vk_device, &create_info, nullptr, &result); vk_result != VK_SUCCESS)
        {
            throw VulkanException(vk_result);
        }
        return result;
    }

    void VulkanDevice::DestroyImageView(VkImageView& view) const noexcept
    {
        m_vkDestroyImageView(m_vk_device, view, nullptr);
        view = VK_NULL_HANDLE;
    }

    void VulkanDevice::QueueSubmit(VkQueue queue, std::span<VkSubmitInfo const> submits, VkFence fence) const
    {
        if (auto vk_result = m_vkQueueSubmit(queue, submits.size(), submits.data(), fence); vk_result != VK_SUCCESS)
        {
            throw VulkanException(vk_result);
        }
    }

    void VulkanDevice::QueueSubmit(VkQueue queue, VkSubmitInfo const& submit, VkFence fence) const
    {
        if (auto vk_result = m_vkQueueSubmit(queue, 1, &submit, fence); vk_result != VK_SUCCESS)
        {
            throw VulkanException(vk_result);
        }
    }

    VkResult VulkanDevice::QueuePresentKHR(VkQueue queue, VkPresentInfoKHR const& present_info) const
    {
        return m_vkQueuePresentKHR(queue, &present_info);
    }

    void VulkanDevice::BeginCommandBuffer(VkCommandBuffer command_buffer, VkCommandBufferBeginInfo const& begin_info) const
    {
        if (auto vk_result = m_vkBeginCommandBuffer(command_buffer, &begin_info); vk_result != VK_SUCCESS)
        {
            throw VulkanException(vk_result);
        }
    }

    void VulkanDevice::EndCommandBuffer(VkCommandBuffer command_buffer) const
    {
        if (auto vk_result = m_vkEndCommandBuffer(command_buffer); vk_result != VK_SUCCESS)
        {
            throw VulkanException(vk_result);
        }
    }

    void VulkanDevice::CmdPipelineBarrier(
        VkCommandBuffer command_buffer,
        VkPipelineStageFlags src_stage_mask,
        VkPipelineStageFlags dst_stage_mask,
        VkDependencyFlags dependency_flags,
        std::span<VkMemoryBarrier const> memory_barriers,
        std::span<VkBufferMemoryBarrier const> buffer_memory_barriers,
        std::span<VkImageMemoryBarrier const> image_memory_barriers) const
    {
        m_vkCmdPipelineBarrier(
            command_buffer,
            src_stage_mask,
            dst_stage_mask,
            dependency_flags,
            memory_barriers.size(),
            memory_barriers.data(),
            buffer_memory_barriers.size(),
            buffer_memory_barriers.data(),
            image_memory_barriers.size(),
            image_memory_barriers.data()
        );
    }

    void VulkanDevice::CmdBeginRendering(VkCommandBuffer command_buffer, VkRenderingInfo const& rendering_info) const
    {
        m_vkCmdBeginRendering(command_buffer, &rendering_info);
    }

    void VulkanDevice::CmdEndRendering(VkCommandBuffer command_buffer) const
    {
        m_vkCmdEndRendering(command_buffer);
    }

    void VulkanDevice::CmdBindPipeline(VkCommandBuffer command_buffer, VkPipelineBindPoint pipeline_bind_point, VkPipeline pipeline) const
    {
        m_vkCmdBindPipeline(command_buffer, pipeline_bind_point, pipeline);
    }

    void VulkanDevice::CmdSetViewport(VkCommandBuffer command_buffer, uint32_t first_viewport, std::span<VkViewport const> viewports) const
    {
        m_vkCmdSetViewport(command_buffer, first_viewport, viewports.size(), viewports.data());
    }

    void VulkanDevice::CmdSetViewport(VkCommandBuffer command_buffer, VkViewport const& viewport) const
    {
        m_vkCmdSetViewport(command_buffer, 0, 1, &viewport);
    }

    void VulkanDevice::CmdSetScissor(VkCommandBuffer command_buffer, uint32_t first_scissor, std::span<VkRect2D const> scissors) const
    {
        m_vkCmdSetScissor(command_buffer, first_scissor, scissors.size(), scissors.data());
    }

    void VulkanDevice::CmdSetScissor(VkCommandBuffer command_buffer, VkRect2D const& scissor) const
    {
        m_vkCmdSetScissor(command_buffer, 0, 1, &scissor);
    }

    void VulkanDevice::CmdDraw(VkCommandBuffer command_buffer, uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance) const
    {
        m_vkCmdDraw(command_buffer, vertex_count, instance_count, first_vertex, first_instance);
    }

    void VulkanDevice::CmdCopyBuffer(VkCommandBuffer command_buffer, VkBuffer src, VkBuffer dst, std::span<VkBufferCopy const> regions) const
    {
        m_vkCmdCopyBuffer(command_buffer, src, dst, regions.size(), regions.data());
    }

    void VulkanDevice::CmdBindVertexBuffers(VkCommandBuffer command_buffer, uint32_t first_binding, uint32_t binding_count, std::span<VkBuffer const> buffers, std::span<VkDeviceSize const> offsets) const
    {
        m_vkCmdBindVertexBuffers(command_buffer, first_binding, binding_count, buffers.data(), offsets.data());
    }

} // Rc