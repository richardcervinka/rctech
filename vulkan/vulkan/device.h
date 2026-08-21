#pragma once

#include "context.h"
#include <chrono>

namespace Rc
{
    class VulkanDevice
    {
    public:
        VulkanDevice(VulkanContext const& context, VkDevice vk_device);

        ~VulkanDevice();

        VulkanDevice(VulkanDevice const&) = delete;
        VulkanDevice(VulkanDevice&&) = delete;
        VulkanDevice& operator=(VulkanDevice const&) = delete;
        VulkanDevice& operator=(VulkanDevice&&) = delete;

        VkDevice Underlying() const { return m_vk_device; }

        operator bool() const noexcept { return m_vk_device != VK_NULL_HANDLE; }

        // vkDeviceWaitIdle
        void WaitIdle() const;

        // vkCreateGraphicsPipelines
        VkPipeline CreateGraphicsPipeline(VkPipelineCache pipeline_cache, VkGraphicsPipelineCreateInfo const& create_info) const;

        // vkDestroyPipeline
        void DestroyPipeline(VkPipeline& pipeline) const noexcept;

        // vkCreateShaderModule
        VkShaderModule CreateShaderModule(VkShaderModuleCreateInfo const& create_info) const;

        // vkDestroyShaderModule
        void DestroyShaderModule(VkShaderModule& shader_module) const noexcept;

        // vkCreateFence
        VkFence CreateFence(VkFenceCreateInfo const& create_info) const;

        // vkDestroyFence
        void DestroyFence(VkFence& fence) const noexcept;

        // vkCreateSemaphore
        VkSemaphore CreateSemaphore(VkSemaphoreCreateInfo const& create_info) const;

        // vkDestroySemaphore
        void DestroySemaphore(VkSemaphore& semaphore) const noexcept;

        // vkWaitForFences
        void WaitForFences(std::span<VkFence const> fences, bool wait_all, uint64_t timeout = UINT64_MAX) const;

        // vkWaitForFences
        void WaitForFence(VkFence const& fence, uint64_t timeout = UINT64_MAX) const;

        // vkResetFences
        void ResetFences(std::span<VkFence> fences) const;

        // vkResetFence
        void ResetFence(VkFence& fence) const;

        // vkAcquireNextImageKHR
        uint32_t AcquireNextImageKHR(VkSwapchainKHR swapchain, uint64_t timeout, VkSemaphore semaphore, VkFence fence) const;

        // vkCreateSwapchainKHR
        VkSwapchainKHR CreateSwapchainKHR(VkSwapchainCreateInfoKHR const& create_info) const;

        // vkDestroySwapchainKHR
        void DestroySwapchainKHR(VkSwapchainKHR swapchain) const noexcept;

        // vkGetSwapchainImagesKHR
        uint32_t GetSwapchainImagesKHRCount(VkSwapchainKHR swapchain) const;

        // vkGetSwapchainImagesKHR
        std::span<VkImage> GetSwapchainImagesKHR(VkSwapchainKHR swapchain, std::span<VkImage> buffer) const;

        // vkCreateCommandPool
        VkCommandPool CreateCommandPool(VkCommandPoolCreateInfo const& create_info) const;

        // vkDestroyCommandPool
        void DestroyCommandPool(VkCommandPool& command_pool) const noexcept;

        // vkAllocateCommandBuffers
        // VkResult AllocateCommandBuffers(VkCommandBufferAllocateInfo const& allocate_info, VkCommandBuffer* pCommandBuffers)

        // vkAllocateCommandBuffers
        VkCommandBuffer AllocateCommandBuffer(VkCommandBufferAllocateInfo const& allocate_info) const;

        //void FreeCommandBuffers(VkCommandPool commandPool, uint32_t commandBufferCount, const VkCommandBuffer* pCommandBuffers);

        // vkFreeCommandBuffers
        void FreeCommandBuffer(VkCommandPool command_pool, VkCommandBuffer& command_buffer) const noexcept;

        // vkResetCommandPool
        void ResetCommandPool(VkCommandPool command_pool, VkCommandPoolResetFlags flags) const;

        // vkCreateImageView
        VkImageView CreateImageView(VkImageViewCreateInfo const& create_info) const;

        // vkDestroyImageView
        void DestroyImageView(VkImageView& view) const noexcept;

        // vkGetDeviceQueue
        VkQueue GetDeviceQueue(uint32_t queue_family_index, uint32_t queue_index) const;

        // vkCreatePipelineLayout
        VkPipelineLayout CreatePipelineLayout(VkPipelineLayoutCreateInfo const& create_info) const;

        // vkDestroyPipelineLayout
        void DestroyPipelineLayout(VkPipelineLayout pipeline_layout) const noexcept;

        // vkQueueSubmit
        void QueueSubmit(VkQueue queue, std::span<VkSubmitInfo const> submits, VkFence fence) const;

        // vkQueueSubmit
        void QueueSubmit(VkQueue queue, VkSubmitInfo const& submit, VkFence fence) const;

        // vkQueuePresentKHR
        VkResult QueuePresentKHR(VkQueue queue, VkPresentInfoKHR const& present_info) const;

        // vkBeginCommandBuffer
        void BeginCommandBuffer(VkCommandBuffer command_buffer, VkCommandBufferBeginInfo const& begin_info) const;

        // vkEndCommandBuffer
        void EndCommandBuffer(VkCommandBuffer command_buffer) const;

        // vkCmdPipelineBarrier
        void CmdPipelineBarrier(
            VkCommandBuffer command_buffer,
            VkPipelineStageFlags src_stage_mask,
            VkPipelineStageFlags dst_stage_mask,
            VkDependencyFlags dependency_flags,
            std::span<VkMemoryBarrier const> memory_barriers,
            std::span<VkBufferMemoryBarrier const> buffer_memory_barriers,
            std::span<VkImageMemoryBarrier const> image_memory_barriers
        ) const;

        // vkCmdPipelineBarrier2
        void CmdPipelineBarrier2(VkCommandBuffer command_buffer, VkDependencyInfo const& dependency_info) const;

        // vkCmdBeginRendering
        void CmdBeginRendering(VkCommandBuffer command_buffer, VkRenderingInfo const& rendering_info) const;

        // vkCmdEndRendering
        void CmdEndRendering(VkCommandBuffer command_buffer) const;

        // vkCmdBindPipeline
        void CmdBindPipeline(VkCommandBuffer command_buffer, VkPipelineBindPoint pipeline_bind_point, VkPipeline pipeline) const;

        // vkCmdSetViewport
        void CmdSetViewport(VkCommandBuffer command_buffer, uint32_t first_viewport, std::span<VkViewport const> viewports) const;

        // vkCmdSetViewport
        void CmdSetViewport(VkCommandBuffer command_buffer, VkViewport const& viewport) const;

        // vkCmdSetScissor
        void CmdSetScissor(VkCommandBuffer command_buffer, uint32_t first_scissor, std::span<VkRect2D const> scissors) const;

        // vkCmdSetScissor
        void CmdSetScissor(VkCommandBuffer command_buffer, VkRect2D const& scissor) const;

        // vkCmdDraw
        void CmdDraw(VkCommandBuffer command_buffer, uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance) const;

        // vkCmdDrawIndexed
        void CmdDrawIndexed(VkCommandBuffer command_buffer, uint32_t index_count, uint32_t instance_count, uint32_t first_index, int32_t vertex_offset, uint32_t first_instance) const;

        // vkCmdCopyBuffer
        void CmdCopyBuffer(VkCommandBuffer command_buffer, VkBuffer src, VkBuffer dst, std::span<VkBufferCopy const> regions) const;

        // vkCmdCopyBufferToImage
        void CmdCopyBufferToImage(VkCommandBuffer command_buffer, VkBuffer src_buffer, VkImage dst_image, VkImageLayout dst_image_layout, std::span<VkBufferImageCopy const> regions) const;

        // vkCmdBindVertexBuffers
        void CmdBindVertexBuffers(VkCommandBuffer command_buffer, uint32_t first_binding, uint32_t binding_count, std::span<VkBuffer const> buffers, std::span<VkDeviceSize const> offsets) const;

        // vkCmdBindIndexBuffer
        void CmdBindIndexBuffer(VkCommandBuffer command_buffer, VkBuffer buffer, VkDeviceSize offset, VkIndexType index_type) const;

        // vkCreateDescriptorSetLayout
        VkDescriptorSetLayout CreateDescriptorSetLayout(VkDescriptorSetLayoutCreateInfo const& create_info) const;

        // vkDestroyDescriptorSetLayout
        void DestroyDescriptorSetLayout(VkDescriptorSetLayout& descriptor_set_layout) const;

        // vkCreateDescriptorPool
        VkDescriptorPool CreateDescriptorPool(VkDescriptorPoolCreateInfo const& create_info) const;

        // vkDestroyDescriptorPool
        void DestroyDescriptorPool(VkDescriptorPool& descriptor_pool) const;

        // vkWriteResourceDescriptorsEXT
        void WriteResourceDescriptorsEXT(std::span<VkResourceDescriptorInfoEXT const> resources, std::span<VkHostAddressRangeEXT const> descriptors) const;

        // vkWriteResourceDescriptorsEXT
        void WriteResourceDescriptorEXT(VkResourceDescriptorInfoEXT const& resource, VkHostAddressRangeEXT const& descriptor) const;

        // vkWriteSamplerDescriptorsEXT
        void WriteSamplerDescriptorsEXT(std::span<VkSamplerCreateInfo const> samplers,  std::span<VkHostAddressRangeEXT const> descriptors) const;

        // vkWriteResourceDescriptorsEXT
        void WriteSamplerDescriptorEXT(VkSamplerCreateInfo const& samplers, VkHostAddressRangeEXT const& descriptor) const;

        // vkGetBufferDeviceAddress
        VkDeviceAddress GetBufferDeviceAddress(VkBufferDeviceAddressInfo const& info) const;

        // vkWaitSemaphores
        VkResult WaitSemaphores(VkSemaphoreWaitInfo const& wait_info, std::chrono::nanoseconds timeout) const;

        // vkGetSemaphoreCounterValue
        uint64_t GetSemaphoreCounterValue(VkSemaphore semaphore) const;

        // vkCmdBindResourceHeapEXT
        void CmdBindResourceHeapEXT(VkCommandBuffer command_buffer, VkBindHeapInfoEXT const& bind_info) const;

        // vkCmdBindSamplerHeapEXT
        void CmdBindSamplerHeapEXT(VkCommandBuffer command_buffer, VkBindHeapInfoEXT const& bind_info) const;

        // vkCmdSetDepthTestEnableEXT
        void CmdSetDepthTestEnableEXT(VkCommandBuffer command_buffer, VkBool32 enable) const;

        // vkCmdSetDepthWriteEnableEXT
        void CmdSetDepthWriteEnableEXT(VkCommandBuffer command_buffer, VkBool32 enable) const;

        // vkCmdSetDepthCompareOpEXT
        void CmdSetDepthCompareOpEXT(VkCommandBuffer command_buffer, VkCompareOp compare_op) const;

        // vkCmdSetStencilTestEnableEXT
        void CmdSetStencilTestEnableEXT(VkCommandBuffer command_buffer, VkBool32 enable) const;

        // vkCmdPushDataEXT
        void CmdPushDataEXT(VkCommandBuffer command_buffer, VkPushDataInfoEXT const& push_data_info) const;

    private:
        friend class VulkanContext;

        template<typename T>
        void Load(char const* name, T& dst)
        {
            dst = reinterpret_cast<T>(m_context.GetDeviceProcAddr(m_vk_device, name));

            if (dst == nullptr)
            {
                throw VulkanLoaderException(name);
            }
        }

        VulkanContext const& m_context;
        VkDevice m_vk_device {VK_NULL_HANDLE};

        PFN_vkDestroyDevice m_vkDestroyDevice {nullptr};
        PFN_vkDeviceWaitIdle m_vkDeviceWaitIdle {nullptr};
        PFN_vkCreateGraphicsPipelines m_vkCreateGraphicsPipelines {nullptr};
        PFN_vkDestroyPipeline m_vkDestroyPipeline {nullptr};
        PFN_vkCreateShaderModule m_vkCreateShaderModule {nullptr};
        PFN_vkDestroyShaderModule m_vkDestroyShaderModule {nullptr};
        PFN_vkCreateFence m_vkCreateFence {nullptr};
        PFN_vkDestroyFence m_vkDestroyFence {nullptr};
        PFN_vkCreateSemaphore m_vkCreateSemaphore {nullptr};
        PFN_vkDestroySemaphore m_vkDestroySemaphore {nullptr};
        PFN_vkWaitForFences m_vkWaitForFences {nullptr};
        PFN_vkResetFences m_vkResetFences {nullptr};
        PFN_vkCreateCommandPool m_vkCreateCommandPool {nullptr};
        PFN_vkDestroyCommandPool m_vkDestroyCommandPool {nullptr};
        PFN_vkAllocateCommandBuffers m_vkAllocateCommandBuffers {nullptr};
        PFN_vkFreeCommandBuffers m_vkFreeCommandBuffers {nullptr};
        PFN_vkResetCommandPool m_vkResetCommandPool {nullptr};
        PFN_vkCreateSwapchainKHR m_vkCreateSwapchainKHR {nullptr};
        PFN_vkDestroySwapchainKHR m_vkDestroySwapchainKHR {nullptr};
        PFN_vkGetSwapchainImagesKHR m_vkGetSwapchainImagesKHR {nullptr};
        PFN_vkAcquireNextImageKHR m_vkAcquireNextImageKHR {nullptr};
        PFN_vkCreateImageView m_vkCreateImageView {nullptr};
        PFN_vkDestroyImageView m_vkDestroyImageView {nullptr};
        PFN_vkGetDeviceQueue m_vkGetDeviceQueue {nullptr};
        PFN_vkCreatePipelineLayout m_vkCreatePipelineLayout {nullptr};
        PFN_vkDestroyPipelineLayout m_vkDestroyPipelineLayout {nullptr};
        PFN_vkQueueSubmit m_vkQueueSubmit {nullptr};
        PFN_vkQueuePresentKHR m_vkQueuePresentKHR {nullptr};
        PFN_vkBeginCommandBuffer m_vkBeginCommandBuffer {nullptr};
        PFN_vkEndCommandBuffer m_vkEndCommandBuffer {nullptr};
        PFN_vkCmdPipelineBarrier m_vkCmdPipelineBarrier {nullptr};
        PFN_vkCmdPipelineBarrier2 m_vkCmdPipelineBarrier2 {nullptr};
        PFN_vkCmdBeginRendering m_vkCmdBeginRendering {nullptr};
        PFN_vkCmdEndRendering m_vkCmdEndRendering {nullptr};
        PFN_vkCmdDraw m_vkCmdDraw {nullptr};
        PFN_vkCmdDrawIndexed m_vkCmdDrawIndexed {nullptr};
        PFN_vkCmdBindPipeline m_vkCmdBindPipeline {nullptr};
        PFN_vkCmdSetViewport m_vkCmdSetViewport {nullptr};
        PFN_vkCmdSetScissor m_vkCmdSetScissor {nullptr};
        PFN_vkCmdCopyBuffer m_vkCmdCopyBuffer {nullptr};
        PFN_vkCmdCopyBufferToImage m_vkCmdCopyBufferToImage {nullptr};
        PFN_vkCmdBindVertexBuffers m_vkCmdBindVertexBuffers {nullptr};
        PFN_vkCmdBindIndexBuffer m_vkCmdBindIndexBuffer {nullptr};
        PFN_vkCreateDescriptorSetLayout m_vkCreateDescriptorSetLayout {nullptr};
        PFN_vkDestroyDescriptorSetLayout m_vkDestroyDescriptorSetLayout {nullptr};
        PFN_vkCreateDescriptorPool m_vkCreateDescriptorPool {nullptr};
        PFN_vkDestroyDescriptorPool m_vkDestroyDescriptorPool {nullptr};
        PFN_vkWriteResourceDescriptorsEXT m_vkWriteResourceDescriptorsEXT {nullptr};
        PFN_vkWriteSamplerDescriptorsEXT m_vkWriteSamplerDescriptorsEXT {nullptr};
        PFN_vkGetBufferDeviceAddress m_vkGetBufferDeviceAddress {nullptr};
        PFN_vkWaitSemaphores m_vkWaitSemaphores {nullptr};
        PFN_vkGetSemaphoreCounterValue m_vkGetSemaphoreCounterValue {nullptr};
        PFN_vkCmdBindResourceHeapEXT m_vkCmdBindResourceHeapEXT {nullptr};
        PFN_vkCmdBindSamplerHeapEXT m_vkCmdBindSamplerHeapEXT {nullptr};
        PFN_vkCmdSetDepthTestEnableEXT m_vkCmdSetDepthTestEnableEXT {nullptr};
        PFN_vkCmdSetDepthWriteEnableEXT m_vkCmdSetDepthWriteEnableEXT {nullptr};
        PFN_vkCmdSetDepthCompareOpEXT m_vkCmdSetDepthCompareOpEXT {nullptr};
        PFN_vkCmdSetStencilTestEnableEXT m_vkCmdSetStencilTestEnableEXT {nullptr};
        PFN_vkCmdPushDataEXT m_vkCmdPushDataEXT {nullptr};
    };

} // Rc