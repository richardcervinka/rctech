#include "swap_chain.h"
#include <cassert>

namespace Rc::Render
{
    SwapChain::SwapChain(VulkanDevice const& vk_device, VkSurfaceKHR surface, Window const& window) :
        vk_device{vk_device}
    {
        auto const extent = window.GetClientArea();
        
        vk_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        vk_info.surface = surface;
        vk_info.minImageCount = 3;
        vk_info.imageFormat = vk_format;
        vk_info.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        vk_info.imageExtent.width = extent.w;
        vk_info.imageExtent.height = extent.h;
        vk_info.imageArrayLayers = 1;
        vk_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        vk_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        vk_info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        vk_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        vk_info.presentMode = VK_PRESENT_MODE_FIFO_KHR;
        vk_info.clipped = VK_TRUE;

        Create();
    }

    SwapChain::~SwapChain()
    {
        vk_device.DestroySwapchainKHR(vk_swap_chain);
    }

    void SwapChain::Resize(int width, int height) // -------------------- add surface parameter
    {
        vk_info.imageExtent.width = static_cast<uint32_t>(width);
        vk_info.imageExtent.height = static_cast<uint32_t>(height);
        vk_info.oldSwapchain = vk_swap_chain;

        Create();
    }

    void SwapChain::Create()
    {
        images.clear();
        views.clear();
        acquire_semaphores.clear();

        vk_swap_chain = vk_device.CreateSwapchainKHR(vk_info);

        auto const images_count = vk_device.GetSwapchainImagesKHRCount(vk_swap_chain);

        std::vector<VkImage> images_buffer(images_count);

        for (auto image : vk_device.GetSwapchainImagesKHR(vk_swap_chain, images_buffer))
        {
            images.emplace_back(image);

            views.push_back(
                std::make_unique<RenderTargetView>(
                    vk_device,
                    image,
                    vk_format,
                    vk_info.imageExtent.width,
                    vk_info.imageExtent.height
                )
            );

            acquire_semaphores.push_back(std::make_unique<Semaphore>(vk_device));
            present_semaphores.push_back(std::make_unique<Semaphore>(vk_device));
        }

        vk_info.oldSwapchain = nullptr;
    }

    void SwapChain::AcquireNextImage()
    {
        acquire_index = (acquire_index + 1) % Size();

        image_index = vk_device.AcquireNextImageKHR(
            vk_swap_chain,
            UINT64_MAX,
            GetAcquireSemaphore().Handle(),
            VK_NULL_HANDLE
        );
    }

    void SwapChain::Present(RenderCommandQueue const& queue) const
    {
        auto present_semaphore = GetPresentSemaphore().Handle();

        VkPresentInfoKHR const present_info
        {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .pNext = nullptr,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &present_semaphore,
            .swapchainCount = 1,
            .pSwapchains = &vk_swap_chain,
            .pImageIndices = &image_index,
            .pResults = nullptr
        };

        auto const vk_result = vk_device.QueuePresentKHR(queue.Handle(), present_info);

        if ((vk_result == VK_ERROR_OUT_OF_DATE_KHR) ||
            (vk_result == VK_SUBOPTIMAL_KHR))
        {
            // Minimal: ignore; a real app by rebuilds swapchain on resize --------------------------------
        }
    }

} // Rc::Render