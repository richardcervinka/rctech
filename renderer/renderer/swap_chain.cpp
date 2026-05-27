#include "swap_chain.h"
#include <cassert>

namespace Rc
{
    SwapChain::SwapChain(VulkanDevice const& vk_device, VkSurfaceKHR surface, Window const& window) :
        m_vk_device{&vk_device}
    {
        auto const extent = window.GetClientArea();
        
        m_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        m_info.surface = surface;
        m_info.minImageCount = 3;
        m_info.imageFormat = m_vk_format;
        m_info.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        m_info.imageExtent.width = extent.w;
        m_info.imageExtent.height = extent.h;
        m_info.imageArrayLayers = 1;
        m_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        m_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        m_info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        m_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        m_info.presentMode = VK_PRESENT_MODE_FIFO_KHR;
        m_info.clipped = VK_TRUE;

        Create();
    }

    SwapChain::~SwapChain()
    {
        if (m_vk_device != nullptr)
        {
            m_vk_device->DestroySwapchainKHR(m_vk_swap_chain);
        }
    }

    void SwapChain::Resize(int width, int height) // -------------------- add surface parameter
    {
        if (m_vk_device != nullptr)
        {
            m_info.imageExtent.width = static_cast<uint32_t>(width);
            m_info.imageExtent.height = static_cast<uint32_t>(height);
            m_info.oldSwapchain = m_vk_swap_chain;

            Create();
        }
    }

    void SwapChain::Create()
    {
        m_images.clear();
        m_acquire_semaphores.clear();

        m_vk_swap_chain = m_vk_device->CreateSwapchainKHR(m_info);

        auto const images_count = m_vk_device->GetSwapchainImagesKHRCount(m_vk_swap_chain);

        std::vector<VkImage> images(images_count);

        for (auto image : m_vk_device->GetSwapchainImagesKHR(m_vk_swap_chain, images))
        {
            m_images.emplace_back(
                *m_vk_device,
                image,
                m_vk_format,
                m_info.imageExtent.width,
                m_info.imageExtent.height
            );

            m_acquire_semaphores.push_back(std::make_unique<Semaphore>(*m_vk_device));
            m_present_semaphores.push_back(std::make_unique<Semaphore>(*m_vk_device));
            m_fences.push_back(std::make_unique<Fence>(*m_vk_device));
        }

        m_info.oldSwapchain = nullptr;
    }

    int SwapChain::AcquireNextImage()
    {
        m_acquire_index = (m_acquire_index + 1) % Count();

        m_fences[m_acquire_index]->Wait();

        m_image_index = m_vk_device->AcquireNextImageKHR(m_vk_swap_chain, UINT64_MAX, GetAcquireSemaphore(), VK_NULL_HANDLE);

        // try {
        //     result = device.acquireNextImageKHR(swapchain, UINT64_MAX, imageAvailableSemaphore, {}, &imageIndex);
        // } catch (vk::OutOfDateKHRError&) {
        //     recreateSwapchain();
        //     return;
        // }

        return m_image_index;
    }

    void SwapChain::Present(VkQueue const& vk_queue) const
    {
        // Sync

        VkPipelineStageFlags stage_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        auto const acquire_semaphore = GetAcquireSemaphore();
        auto const present_semaphore = GetPresentSemaphore();

        VkSubmitInfo const submit_info
        {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext = nullptr,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &acquire_semaphore,
            .pWaitDstStageMask = &stage_flags,
            .commandBufferCount = 0,
            .pCommandBuffers = nullptr,
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = &present_semaphore
        };

        m_vk_device->QueueSubmit(vk_queue, submit_info, GetFence());

        // Present

        VkPresentInfoKHR const present_info
        {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .pNext = nullptr,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &present_semaphore,
            .swapchainCount = 1,
            .pSwapchains = &m_vk_swap_chain,
            .pImageIndices = &m_image_index,
            .pResults = nullptr
        };

        auto const vk_result = m_vk_device->QueuePresentKHR(vk_queue, present_info);

        if ((vk_result == VK_ERROR_OUT_OF_DATE_KHR) ||
            (vk_result == VK_SUBOPTIMAL_KHR))
        {
            // Minimal: ignore; a real app by rebuilds swapchain on resize --------------------------------
        }
    }

} // Rc