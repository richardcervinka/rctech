#include "instance.h"
#include "platform/log.h"
#include <iostream>

namespace Rc::Render
{
    Instance::~Instance()
    {
        m_instance->DestroyDebugUtilsMessengerEXT(m_vk_debug_msg);
    }

    Instance::Instance()
    {
        m_context = std::make_unique<VulkanContext>("vulkan-1.dll");

        auto const api_version = m_context->EnumerateInstanceVersion();

        auto const extensions = EnumerateExtensions();

        // Check presence of mandatory extensions.
        if (!extensions.contains(VK_KHR_SURFACE_EXTENSION_NAME))
        {
            throw std::runtime_error("Required VK_KHR_surface");
        }
        if (!extensions.contains(VK_KHR_WIN32_SURFACE_EXTENSION_NAME))
        {
            throw std::runtime_error("Required VK_KHR_win32_surface");
        }
        if (!extensions.contains(VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
        {
            throw std::runtime_error("EXTDebugUtilsExtensionName");
        }
        // if (!extensions.contains(vk::KHRSpirv14ExtensionName))
        // {
        //     throw std::runtime_error("vk::KHRSpirv14ExtensionName");
        // }

        std::vector<char const*> enable_extensions
        {
            VK_KHR_SURFACE_EXTENSION_NAME,
            VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME
            //vk::KHRSpirv14ExtensionName
            //vk::KHRSynchronization2ExtensionName,
            //vk::KHRCreateRenderpass2ExtensionName
        };

        std::vector<char const*> enable_layers;

        enable_layers.push_back("VK_LAYER_KHRONOS_validation"); //------------------------------ check if contains

        VkApplicationInfo const app_info
        {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext = nullptr,
            .pApplicationName = "tride",
            .applicationVersion = 0,
            .pEngineName = "tride",
            .engineVersion = 0,
            .apiVersion = VK_API_VERSION_1_4
        };

        VkInstanceCreateInfo const instance_info
        {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .pApplicationInfo = &app_info,
            .enabledLayerCount = static_cast<uint32_t>(enable_layers.size()),
            .ppEnabledLayerNames = enable_layers.data(),
            .enabledExtensionCount = static_cast<uint32_t>(enable_extensions.size()),
            .ppEnabledExtensionNames = enable_extensions.data()
        };

        m_instance = m_context->CreateInstance(instance_info);
    }

    std::map<std::string, VulkanVersion> Instance::EnumerateExtensions() const
    { 
        auto const count = m_context->EnumerateInstanceExtensionPropertiesCount();

        std::vector<VkExtensionProperties> buffer(count);

        std::map<std::string, VulkanVersion> result;

        for (auto const& p : m_context->EnumerateInstanceExtensionProperties(buffer))
        {
            result[p.extensionName] = {p.specVersion};
        }

        return result;
    }

    void Instance::EnableValidation()
    {
        VkDebugUtilsMessengerCreateInfoEXT const create_info
        {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            .pNext = nullptr,
            .flags = 0,
            .messageSeverity = VkDebugUtilsMessageSeverityFlagsEXT{
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
            },
            .messageType = VkDebugUtilsMessageSeverityFlagsEXT{
                VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
            },
            .pfnUserCallback = VulkanDebugCallback,
            .pUserData = nullptr
        };
        
        m_vk_debug_msg = m_instance->CreateDebugUtilsMessengerEXT(create_info);
    }

    std::vector<std::unique_ptr<Adapter>> Instance::EnumerateAdapters()
    {
        std::vector<VkPhysicalDevice> devices(m_instance->EnumeratePhysicalDevicesCount());
        std::vector<std::unique_ptr<Adapter>> adapters;

        for (auto& device : m_instance->EnumeratePhysicalDevices(devices))
        {
            adapters.push_back(std::make_unique<Adapter>(*m_context, *m_instance, device));
        }

        return adapters;
    }

    std::unique_ptr<Surface> Instance::CreateSurface(Window const& window)
    {
        #ifdef VK_USE_PLATFORM_WIN32_KHR
        return std::make_unique<Surface>(*m_instance, window);
        #endif
    }

    VkBool32 Instance::VulkanDebugCallback(
        [[maybe_unused]] VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
        [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT message_types,
        VkDebugUtilsMessengerCallbackDataEXT const* callback_data,
        [[maybe_unused]] void* user_data)
    {
        Log::Debug(std::format("Validation Layer: {}", callback_data->pMessage));
        return VK_FALSE;  
    }

} // Rc::Render