#pragma once

#include "vulkan/device.h"
#include "vulkan/instance.h"
#include "fence.h"
#include "command_buffer.h"
#include "buffer.h"

namespace Rc::Render
{
    // DescriptorFactory

    class ResourceDescriptorBuilder // ----------------------- mel by take generovat layout pro pipeline state
    {
    public:

        ResourceDescriptorBuilder(
            VulkanInstance const& instance,
            VulkanDevice const& device,
            VkPhysicalDevice vk_physical_device
        );

        //void Attach(std::shared_ptr<Buffer> host_buffer);

        void CreateUniformBuffer(uint32_t slot, Buffer const& buffer, Buffer& host_buffer, BufferRegion host_region);

        // Write(std::shared_ptr<Buffer> host_buffer)

    private:
        VulkanDevice const* m_vk_device {nullptr};

        //std::shared_ptr<Buffer> m_buffer;

        uint32_t m_stride {0};
    };

    // class SamplerDescriptorHeap

} // Rc::Render