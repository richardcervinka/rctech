#pragma once

#include "vulkan/device.h"
#include "command_list.h"
#include "swap_chain.h"
#include "fence.h"
#include <span>

namespace Rc
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

        void Submit(CommandBuffer const& cb, Fence const& fence);

        void Present(SwapChain const& sc) const;

        uint32_t FamilyIndex() const { return m_vk_family; }

    private:
        VulkanDevice const* m_vk_device {nullptr};
        uint32_t m_vk_family {};
        VkQueue m_vk_queue {VK_NULL_HANDLE};
    };

} // Rc