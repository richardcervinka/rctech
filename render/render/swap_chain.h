#pragma once

#include "vulkan/device.h"
#include "platform/window.h"
#include "render_target.h"
#include "semaphore.h"
#include "texture.h"
#include "command_queue.h"

namespace Rc::Render
{
    enum class ColorProfile
    {
        SDR,
        HDR10
    };

    class SwapChain
    {
    public:
        ~SwapChain();

        SwapChain(
            VulkanDevice const& vk_device,
            VkSurfaceKHR surface,
            Window const& window,
            VkSurfaceFormatKHR const& surface_format
        );

        SwapChain(SwapChain const&) = delete;
        SwapChain& operator=(SwapChain const&) = delete;
        SwapChain(SwapChain&& other) = delete;
        SwapChain& operator=(SwapChain&& other) = delete;

        void Resize(int width, int height); // uint32_t ? Window ?

        int Width() const
        {
            return static_cast<int>(vk_info.imageExtent.width);
        }
        
        int Height() const
        {
            return static_cast<int>(vk_info.imageExtent.height);
        }

        Rectangle<int> RenderArea() const
        {
            return {0, 0, Width(), Height()};
        }

        // Get number of images.
        int Size()
        {
            return static_cast<int>(images.size());
        }

        void AcquireNextImage();

        RenderTargetView const& GetRenderTargetView() const
        {
            return *back_buffers.at(image_index).render_target;
        }

        // Texture2D const& GetTexture() const // ------------------------ rename to framebuffer
        // {
        //     return m_images.at(m_image_index);
        // }

        uint32_t Index() const
        {
            return image_index;
        }

        PixelFormat Format() const;

        void Present(RenderCommandQueue const& queue) const;

        Semaphore const& GetAcquireSemaphore() const
        {
            return *acquire_semaphores[acquire_index];
        }

        Semaphore const& GetPresentSemaphore() const
        {
            return *present_semaphores[image_index];
        }

    private:
        void Create();

        VulkanDevice const& vk_device;

        VkSwapchainCreateInfoKHR vk_info {};

        VkFormat vk_format {VK_FORMAT_UNDEFINED};

        VkSwapchainKHR vk_swap_chain {VK_NULL_HANDLE};

        // Indexed by the m_index.
        //std::vector<Texture2D> images;
        std::vector<VkImage> images;

        struct BackBuffer
        {
            std::unique_ptr<Texture2d> texture;
            std::unique_ptr<RenderTargetView> render_target;
        };

        // Indexed by the m_index.
        std::vector<BackBuffer> back_buffers;

        std::vector<std::unique_ptr<Semaphore>> acquire_semaphores;

        // Indexed by the m_index.
        std::vector<std::unique_ptr<Semaphore>> present_semaphores;

        int acquire_index {-1};

        uint32_t image_index {0};
    };

} // Rc::Render