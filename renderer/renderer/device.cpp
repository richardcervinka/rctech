#include "device.h"
#include "error.h"
#include <iostream>
#include <map>

namespace Rc
{
    Device::Device(
        VulkanContext const& context,
        VulkanInstance const& instance,
        VkPhysicalDevice vk_physical_device, 
        Surface const& surface
    ) :
        m_instance{&instance},
        m_vk_physical_device{vk_physical_device}
    {
        // Setup extensions

        auto const extensions = EnumerateExtensions();

        // Mandatory extensions...

        if (!extensions.contains(VK_KHR_SWAPCHAIN_EXTENSION_NAME))
        {
            throw std::runtime_error("Required VK_KHR_swapchain");
        }

        std::vector<char const*> enable_extensions
        {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        // Optional extensions...

        VmaAllocatorCreateFlags allocator_ext_flags = 0;

        if (extensions.contains(VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME))
        {
            enable_extensions.push_back(VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME);
            allocator_ext_flags |= VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT;
        }
        if (extensions.contains(VK_KHR_BIND_MEMORY_2_EXTENSION_NAME))
        {
            enable_extensions.push_back(VK_KHR_BIND_MEMORY_2_EXTENSION_NAME);
            allocator_ext_flags |= VMA_ALLOCATOR_CREATE_KHR_BIND_MEMORY2_BIT;
        }
        if (extensions.contains(VK_KHR_MAINTENANCE_4_EXTENSION_NAME))
        {
            enable_extensions.push_back(VK_KHR_MAINTENANCE_4_EXTENSION_NAME);
            allocator_ext_flags |= VMA_ALLOCATOR_CREATE_KHR_MAINTENANCE4_BIT;
        }
        if (extensions.contains(VK_KHR_MAINTENANCE_5_EXTENSION_NAME))
        {
            enable_extensions.push_back(VK_KHR_MAINTENANCE_5_EXTENSION_NAME);
            allocator_ext_flags |= VMA_ALLOCATOR_CREATE_KHR_MAINTENANCE5_BIT;
        }
        if (extensions.contains(VK_EXT_MEMORY_BUDGET_EXTENSION_NAME))
        {
            enable_extensions.push_back(VK_EXT_MEMORY_BUDGET_EXTENSION_NAME);
            allocator_ext_flags |= VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
        }
        if (extensions.contains(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME))
        {
            enable_extensions.push_back(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME);
            allocator_ext_flags |= VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
        }
        if (extensions.contains(VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME))
        {
            enable_extensions.push_back(VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME);
            allocator_ext_flags |= VMA_ALLOCATOR_CREATE_EXT_MEMORY_PRIORITY_BIT;
        }
        if (extensions.contains(VK_AMD_DEVICE_COHERENT_MEMORY_EXTENSION_NAME))
        {
            enable_extensions.push_back(VK_AMD_DEVICE_COHERENT_MEMORY_EXTENSION_NAME);
            allocator_ext_flags |= VMA_ALLOCATOR_CREATE_AMD_DEVICE_COHERENT_MEMORY_BIT;
        }
        if (extensions.contains(VK_KHR_EXTERNAL_MEMORY_WIN32_EXTENSION_NAME))
        {
            enable_extensions.push_back(VK_KHR_EXTERNAL_MEMORY_WIN32_EXTENSION_NAME);
            allocator_ext_flags |= VMA_ALLOCATOR_CREATE_KHR_EXTERNAL_MEMORY_WIN32_BIT;
        }

        // [graphics, transfer]
        float gueue_priorities[] = {1.0};

        // [graphics, transfer]
        std::array<VkDeviceQueueCreateInfo, 2> queue_info {};

        uint32_t queue_info_count = 0;

        auto const family_properties = GetQueueFamilyProperties();

        // Find render queue (GRAPHICS + TRANSFER + SURFACE)
        for (uint32_t family = 0; family < family_properties.size(); family++)
        {
            auto const& properties = family_properties[family];

            if ((properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) &&
                (properties.queueFlags & VK_QUEUE_TRANSFER_BIT))
            {
                if (!m_instance->GetPhysicalDevicePresentationSupport(vk_physical_device, family))
                {
                    continue;
                }

                if (m_instance->GetPhysicalDeviceSurfaceSupportKHR(vk_physical_device, family, surface.m_vk_surface))
                {
                    queue_info[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                    queue_info[0].queueFamilyIndex = family;
                    queue_info[0].pQueuePriorities = gueue_priorities;
                    queue_info[0].queueCount = 1;
                    queue_info_count = 1;

                    m_vk_graphics_queue_family = family;

                    break;
                }
            }
        }

        if (queue_info_count == 0)
        {
            throw std::runtime_error("No graphics queue family found!");
        }

        // Try to find dedicated transfer queue (optional)
        for (uint32_t family = 0; family < family_properties.size(); family++)
        {
            auto const& p = family_properties[family];

            if (p.queueFlags == VK_QUEUE_TRANSFER_BIT)
            {
                queue_info[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                queue_info[1].queueFamilyIndex = family;
                queue_info[1].pQueuePriorities = gueue_priorities; // --------------- review
                queue_info[1].queueCount = 1;
                queue_info_count = 2;

                m_vk_graphics_queue_family = family;

                break;
            }
        }

        // Device features...

        VkPhysicalDeviceBufferDeviceAddressFeatures device_address_features
        {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES,
            .pNext = nullptr,
            .bufferDeviceAddress = VK_TRUE
        };

        VkPhysicalDeviceDynamicRenderingFeatures dynamic_rendering_features
        {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES,
            .pNext = &device_address_features,
            .dynamicRendering = VK_TRUE
        };

        VkPhysicalDeviceSynchronization2Features synchronization2_features
        {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES,
            .pNext = &dynamic_rendering_features,
            .synchronization2 = VK_TRUE
        };

        void* features = &synchronization2_features;

        VkDeviceCreateInfo info
        {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = features,
            .flags = {},
            .queueCreateInfoCount = queue_info_count,
            .pQueueCreateInfos = queue_info.data(),
            .enabledLayerCount = 0,
            .ppEnabledLayerNames = nullptr,
            .enabledExtensionCount = static_cast<uint32_t>(enable_extensions.size()),
            .ppEnabledExtensionNames = enable_extensions.data()
        };

        m_device = m_instance->CreateDevice(vk_physical_device, info);

        auto const vma_functions = context.GetVmaFunctions(instance, *m_device);

        VmaAllocatorCreateInfo allocator_info {};
        allocator_info.instance = m_instance->Handle();
        allocator_info.device = m_device->Handle();
        allocator_info.physicalDevice = m_vk_physical_device;
        allocator_info.vulkanApiVersion = VK_API_VERSION_1_4; //------------------------------- Precist z instance
        allocator_info.pVulkanFunctions = &vma_functions;
        allocator_info.flags = allocator_ext_flags;
    
        if (auto vk_result = vmaCreateAllocator(&allocator_info, &m_vma_allocator); vk_result != VK_SUCCESS)
        {
            throw VulkanError(vk_result);
        }
    }

    std::map<std::string, VulkanVersion> Device::EnumerateExtensions() const
    {
        std::map<std::string, VulkanVersion> result;

        auto const count = m_instance->EnumerateDeviceExtensionPropertiesCount(m_vk_physical_device, {});

        std::vector<VkExtensionProperties> properties(count);

        for (auto const& extension : m_instance->EnumerateDeviceExtensionProperties(m_vk_physical_device, {}, properties))
        {
            result[extension.extensionName] = {extension.specVersion};
        }

        return result;
    }

    std::vector<VkQueueFamilyProperties> Device::GetQueueFamilyProperties() const
    {
        auto const count = m_instance->GetPhysicalDeviceQueueFamilyPropertiesCount(m_vk_physical_device);

        std::vector<VkQueueFamilyProperties> result(count);

        auto const span = m_instance->GetPhysicalDeviceQueueFamilyProperties(m_vk_physical_device, result);

        result.resize(span.size());

        return result; 
    }

    Device::~Device()
    {
        WaitIdle();

        if (m_vma_allocator != VK_NULL_HANDLE)
        {
            vmaDestroyAllocator(m_vma_allocator);
        }
    }

    std::unique_ptr<SwapChain> Device::CreateSwapChain(Surface const& surface, Window const& window)
    {
        return std::make_unique<SwapChain>(*m_device, surface.m_vk_surface, window);
    }

    std::unique_ptr<Shader> Device::CreateShader(std::span<uint32_t const> spirv)
    {
        return std::make_unique<Shader>(*m_device, spirv);
    }

    std::unique_ptr<CommandQueue> Device::CreateGraphicsQueue()
    {
        return std::make_unique<CommandQueue>(*m_device, m_vk_graphics_queue_family, 0);
    }

    std::unique_ptr<Fence> Device::CreateFence()
    {
        return std::make_unique<Fence>(*m_device);
    }

    std::unique_ptr<Semaphore> Device::CreateSemaphore() const
    {
        return std::make_unique<Semaphore>(*m_device);
    }

    void Device::WaitIdle() const noexcept
    {
        if (m_device)
        {
            m_device->WaitIdle();
        }
    }

    std::shared_ptr<PipelineLayout> Device::CreatePipelineLayout()
    {
        return std::make_shared<PipelineLayout>(*m_device);
    }

    PipelineFactory Device::CreatePipelineFactory()
    {
        return PipelineFactory(*m_device);
    }

    std::unique_ptr<VertexBuffer> Device::AllocateVertexBuffer(std::size_t size) const
    {
        return std::make_unique<VertexBuffer>(m_vma_allocator, size);
    }

    std::unique_ptr<StagingBuffer> Device::AllocateStagingBuffer(std::size_t size) const
    {
        return std::make_unique<StagingBuffer>(m_vma_allocator, size);
    }

} // Rc