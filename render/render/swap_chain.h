#pragma once

#include "vulkan/device.h"
#include "platform/window.h"
#include "texture.h"
#include "semaphore.h"
#include "command_queue.h"

namespace Rc::Render
{
    class SwapChain
    {
    public:
        ~SwapChain();

        SwapChain(VulkanDevice const& vk_device, VkSurfaceKHR surface, Window const& window);

        SwapChain(SwapChain const&) = delete;
        SwapChain& operator=(SwapChain const&) = delete;
        SwapChain(SwapChain&& other) = delete;
        SwapChain& operator=(SwapChain&& other) = delete;

        void Resize(int width, int height); // uint32_t ? Window ?

        int Width() const
        {
            return m_info.imageExtent.width;
        }
        
        int Height() const
        {
            return m_info.imageExtent.height;
        }

        Rectangle<int> RenderArea() const
        {
            return {0, 0, Width(), Height()};
        }

        // Get number of images.
        int Count()
        {
            return m_images.size();
        }

        void AcquireNextImage();

        RenderTargetView const& GetRenderTargetView() const
        {
            return *m_views.at(m_image_index);
        }

        // Texture2D const& GetTexture() const // ------------------------ rename to framebuffer
        // {
        //     return m_images.at(m_image_index);
        // }

        uint32_t Index() const { return m_image_index; }

        VkFormat GetFormat() const { return m_vk_format; }

        void Present(RenderCommandQueue const& queue) const;

        Semaphore const& GetAcquireSemaphore() const
        {
            return *m_acquire_semaphores[m_acquire_index];
        }

        Semaphore const& GetPresentSemaphore() const
        {
            return *m_present_semaphores[m_image_index];
        }

    private:
        void Create();

        VulkanDevice const* m_vk_device {nullptr};

        VkSwapchainCreateInfoKHR m_info {};

        VkFormat m_vk_format {VK_FORMAT_R8G8B8A8_SRGB}; // -------------------- determine!

        VkSwapchainKHR m_vk_swap_chain {VK_NULL_HANDLE};

        // Indexed by the m_index.
        std::vector<Texture2D> m_images;

        // Indexed by the m_index.
        std::vector<std::unique_ptr<RenderTargetView>> m_views;

        std::vector<std::unique_ptr<Semaphore>> m_acquire_semaphores;

        // Indexed by the m_index.
        std::vector<std::unique_ptr<Semaphore>> m_present_semaphores;

        int m_acquire_index {-1};

        uint32_t m_image_index {0};
    };

} // Rc::Render