#pragma once

#include "vulkan/instance.h"
#include <vector>
#include <map>
#include "adapter.h"
#include <Windows.h>
#include "platform/window.h"
#include "surface.h"

namespace Rc::Render
{   
    // Graphics infrastructure context
    class Instance
    {
    public:
        Instance();
        ~Instance();

        Instance(Instance const&) = delete;
        Instance& operator=(Instance const&) = delete;
        Instance(Instance&& other) = delete;
        Instance& operator=(Instance&& other) = delete;

        std::vector<std::unique_ptr<Adapter>> EnumerateAdapters();

        std::unique_ptr<Surface> CreateSurface(Window const& window);

        void EnableValidation();

        std::map<std::string, VulkanVersion> EnumerateExtensions() const;

        Version ApiVersion() const
        {
            return {1, 4};
        }

    private:
        static VkBool32 VulkanDebugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
            VkDebugUtilsMessageTypeFlagsEXT message_types,
            VkDebugUtilsMessengerCallbackDataEXT const* callback_data,
            void* user_data
        );

        std::unique_ptr<VulkanContext> context;
        std::unique_ptr<VulkanInstance> instance;

        VkDebugUtilsMessengerEXT vk_debug_msg {VK_NULL_HANDLE};
    };

} // Rc::Render