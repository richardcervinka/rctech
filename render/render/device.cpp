#include "device.h"
#include "error.h"
#include <map>
#include <array>
#include <span>

namespace Rc::Render
{
    // Find render queue (graphics + transfer + surface + presentation)
    static std::optional<uint32_t> FindRenderQueueFamilyIndex(std::span<QueueFamilyProperties const> properties)
    {
        for (uint32_t family_index = 0; family_index < properties.size(); family_index++)
        {
            if (properties[family_index].graphics &&
                properties[family_index].transfer &&
                properties[family_index].presentation &&
                properties[family_index].surface)
            {
                return family_index;
            }
        }

        return std::nullopt;
    }

    // Try to find dedicated transfer queue (optional)
    static std::optional<uint32_t> FindDedicatedTransferQueueFamilyIndex(std::span<QueueFamilyProperties const> properties)
    {
        for (uint32_t family_index = 0; family_index < properties.size(); family_index++)
        {
            if (!properties[family_index].graphics &&
                properties[family_index].transfer &&
                !properties[family_index].compute)
            {
                return family_index;
            }
        }

        return std::nullopt;
    }

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
            throw std::runtime_error("Required " VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        }
        if (!extensions.contains(VK_EXT_DESCRIPTOR_HEAP_EXTENSION_NAME))
        {
            throw std::runtime_error("Required " VK_EXT_DESCRIPTOR_HEAP_EXTENSION_NAME);
        }
        if (!extensions.contains(VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME))
        {
            throw std::runtime_error("Required " VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME);
        }

        std::vector<char const*> enable_extensions
        {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            VK_EXT_DESCRIPTOR_HEAP_EXTENSION_NAME,
            VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME
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

        auto const family_properties = GetQueueFamilyProperties(surface);

        auto const graphics_queue_family_index = FindRenderQueueFamilyIndex(family_properties);

        // Graphics queue is required.
        if (!graphics_queue_family_index)
        {
            throw std::runtime_error("No graphics queue family found!");
        }

        // Try to find dedicated transfer queue (optional)
        auto const transfer_queue_family_index = FindDedicatedTransferQueueFamilyIndex(family_properties);

        // [graphics, transfer]
        std::array<float, 2> gueue_priorities = {1.0, 1.0};

        // [graphics, transfer]
        std::vector<VkDeviceQueueCreateInfo> queue_info;

        if (transfer_queue_family_index)
        {
            m_vk_graphics_queue_family = {*graphics_queue_family_index, 0};
            m_vk_transfer_queue_family = {*transfer_queue_family_index, 0};

            queue_info = {
                {
                    .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                    .pNext = nullptr,
                    .flags = {},
                    .queueFamilyIndex = *graphics_queue_family_index,
                    .queueCount = 1,
                    .pQueuePriorities = gueue_priorities.data()
                },
                {
                    .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                    .pNext = nullptr,
                    .flags = {},
                    .queueFamilyIndex = *transfer_queue_family_index,
                    .queueCount = 1,
                    .pQueuePriorities = gueue_priorities.data()
                }
            };
        }
        else if (family_properties[*graphics_queue_family_index].count > 1)
        {
            m_vk_graphics_queue_family = {*graphics_queue_family_index, 0};
            m_vk_transfer_queue_family = {*graphics_queue_family_index, 1};

            queue_info = {
                {
                    .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                    .pNext = nullptr,
                    .flags = {},
                    .queueFamilyIndex = *graphics_queue_family_index,
                    .queueCount = 2,
                    .pQueuePriorities = gueue_priorities.data()
                }
            };
        }
        else
        {
            m_vk_graphics_queue_family = {*graphics_queue_family_index, 0};
            m_vk_transfer_queue_family = {*graphics_queue_family_index, 0};

            queue_info = {
                {
                    .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                    .pNext = nullptr,
                    .flags = {},
                    .queueFamilyIndex = *graphics_queue_family_index,
                    .queueCount = 1,
                    .pQueuePriorities = gueue_priorities.data()
                }
            };

            assert(false && "fallback!");
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
        VkPhysicalDeviceDescriptorHeapFeaturesEXT descriptor_heap_features_ext
        {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_HEAP_FEATURES_EXT,
            .pNext = &synchronization2_features,
            .descriptorHeap = VK_TRUE,
            .descriptorHeapCaptureReplay = VK_FALSE
        };
        VkPhysicalDeviceTimelineSemaphoreFeatures timeline_semaphore_features
        {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES,
            .pNext = &descriptor_heap_features_ext,
            .timelineSemaphore = VK_TRUE
        };

        void* features = &timeline_semaphore_features;

        VkDeviceCreateInfo info
        {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = features,
            .flags = {},
            .queueCreateInfoCount = static_cast<uint32_t>(queue_info.size()),
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
            throw VulkanException(vk_result);
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

    std::vector<QueueFamilyProperties> Device::GetQueueFamilyProperties(Surface const& surface) const
    {
        auto const count = m_instance->GetPhysicalDeviceQueueFamilyPropertiesCount(m_vk_physical_device);
        std::vector<VkQueueFamilyProperties> properties(count);
        m_instance->GetPhysicalDeviceQueueFamilyProperties(m_vk_physical_device, properties);

        std::vector<QueueFamilyProperties> result;
        result.reserve(count);

        for (std::size_t index = 0; index < properties.size(); index++)
        {
            result.push_back({
                .index = static_cast<uint32_t>(index),
                .count = properties[index].queueCount,
                .graphics = (properties[index].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0,
                .transfer = (properties[index].queueFlags & VK_QUEUE_TRANSFER_BIT) != 0,
                .compute = (properties[index].queueFlags & VK_QUEUE_COMPUTE_BIT) != 0,
                .presentation = m_instance->GetPhysicalDevicePresentationSupport(m_vk_physical_device, index),
                .surface = m_instance->GetPhysicalDeviceSurfaceSupportKHR(m_vk_physical_device, index, surface.Handle())
            });
        }

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

    std::unique_ptr<RenderCommandQueue> Device::CreateGraphicsQueue()
    {
        return std::make_unique<RenderCommandQueue>(*m_device, m_vk_graphics_queue_family.first, m_vk_graphics_queue_family.second);
    }

    std::unique_ptr<TransferCommandQueue> Device::CreateTransferQueue()
    {
        return std::make_unique<TransferCommandQueue>(*m_device, m_vk_transfer_queue_family.first, m_vk_transfer_queue_family.second);
    }

    std::unique_ptr<Fence> Device::CreateFence()
    {
        return std::make_unique<Fence>(*m_device);
    }

    std::unique_ptr<Semaphore> Device::CreateSemaphore() const
    {
        return std::make_unique<Semaphore>(*m_device);
    }

    std::unique_ptr<TimelineSemaphore> Device::CreateTimelineSemaphore() const
    {
        return std::make_unique<TimelineSemaphore>(*m_device);
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

    ResourceDescriptorBuilder Device::CreateResourceDescriptorBuilder()
    {
        return {*m_instance, *m_device, m_vk_physical_device};
    }

    std::unique_ptr<Buffer> Device::AllocateVertexBuffer(uint64_t size) const
    {
        return std::make_unique<Buffer>(*m_device, VertexBufferInfo{size}, m_vma_allocator);
    }

    std::unique_ptr<Buffer> Device::AllocateIndexBuffer(uint64_t size) const
    {
        return std::make_unique<Buffer>(*m_device, IndexBufferInfo{size}, m_vma_allocator);
    }

    std::unique_ptr<Buffer> Device::AllocateStagingBuffer(uint64_t size) const
    {
        return std::make_unique<Buffer>(*m_device, StagingBufferInfo{size}, m_vma_allocator);
    }

    std::unique_ptr<Buffer> Device::AllocateUniformBuffer(uint64_t size) const
    {
        return std::make_unique<Buffer>(*m_device, UniformBufferInfo{size}, m_vma_allocator);
    }

    std::unique_ptr<Buffer> Device::AllocateDescriptorHeapBuffer(uint64_t size) const
    {
        return std::make_unique<Buffer>(*m_device, DescriptorHeapBufferInfo{size}, m_vma_allocator);
    }

} // Rc::Render