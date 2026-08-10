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
        for (uint32_t i = 0; i < properties.size(); i++)
        {
            if ((properties[i].graphics == false) &&
                (properties[i].transfer == true) &&
                (properties[i].compute == false))
            {
                return i;
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
        instance{&instance},
        vk_physical_device{vk_physical_device}
    {
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
        if (!extensions.contains(VK_KHR_SHADER_UNTYPED_POINTERS_EXTENSION_NAME))
        {
            throw std::runtime_error("Required " VK_KHR_SHADER_UNTYPED_POINTERS_EXTENSION_NAME);
        }
        if (!extensions.contains(VK_EXT_DYNAMIC_RENDERING_UNUSED_ATTACHMENTS_EXTENSION_NAME))
        {
            throw std::runtime_error("Required " VK_EXT_DYNAMIC_RENDERING_UNUSED_ATTACHMENTS_EXTENSION_NAME);
        }
        if (!extensions.contains(VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME))
        {
            throw std::runtime_error("Required " VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME);
        }
        if (!extensions.contains(VK_EXT_EXTENDED_DYNAMIC_STATE_3_EXTENSION_NAME))
        {
            throw std::runtime_error("Required " VK_EXT_EXTENDED_DYNAMIC_STATE_3_EXTENSION_NAME);
        }

        std::vector<char const*> enable_extensions
        {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            VK_EXT_DESCRIPTOR_HEAP_EXTENSION_NAME,
            VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME,
            VK_KHR_SHADER_UNTYPED_POINTERS_EXTENSION_NAME,
            VK_EXT_DYNAMIC_RENDERING_UNUSED_ATTACHMENTS_EXTENSION_NAME,
            VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME,
            VK_EXT_EXTENDED_DYNAMIC_STATE_3_EXTENSION_NAME
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

        // Device features...

        VkPhysicalDeviceExtendedDynamicState3FeaturesEXT dynamic_state_3_features
        {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_3_FEATURES_EXT,
            .pNext = nullptr
        };
        VkPhysicalDeviceExtendedDynamicStateFeaturesEXT dynamic_state_features
        {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT,
            .pNext = &dynamic_state_3_features,
            .extendedDynamicState = VK_TRUE
        };
        VkPhysicalDeviceBufferDeviceAddressFeatures device_address_features
        {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES,
            .pNext = &dynamic_state_features,
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
        VkPhysicalDeviceDynamicRenderingUnusedAttachmentsFeaturesEXT unused_attachments_features
        {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_UNUSED_ATTACHMENTS_FEATURES_EXT,
            .pNext = &timeline_semaphore_features,
            .dynamicRenderingUnusedAttachments = VK_TRUE
        };
        VkPhysicalDeviceVulkan14Features features_14
        {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES,
            .pNext = &unused_attachments_features,
            .globalPriorityQuery = VK_FALSE,
            .shaderSubgroupRotate = VK_FALSE,
            .shaderSubgroupRotateClustered = VK_FALSE,
            .shaderFloatControls2 = VK_FALSE,
            .shaderExpectAssume = VK_FALSE,
            .rectangularLines = VK_FALSE,
            .bresenhamLines = VK_FALSE,
            .smoothLines = VK_FALSE,
            .stippledRectangularLines = VK_FALSE,
            .stippledBresenhamLines = VK_FALSE,
            .stippledSmoothLines = VK_FALSE,
            .vertexAttributeInstanceRateDivisor = VK_FALSE,
            .vertexAttributeInstanceRateZeroDivisor = VK_FALSE,
            .indexTypeUint8 = VK_FALSE,
            .dynamicRenderingLocalRead = VK_FALSE,
            .maintenance5 = VK_FALSE,
            .maintenance6 = VK_FALSE,
            .pipelineProtectedAccess = VK_FALSE,
            .pipelineRobustness = VK_FALSE,
            .hostImageCopy = VK_FALSE,
            .pushDescriptor  = VK_FALSE
        };
        VkPhysicalDeviceShaderUntypedPointersFeaturesKHR shader_untyped_pointers_features
        {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_UNTYPED_POINTERS_FEATURES_KHR,
            .pNext = &features_14,
            .shaderUntypedPointers = VK_TRUE
        };

        void* features = &shader_untyped_pointers_features;

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
            vk_graphics_queue_family = {*graphics_queue_family_index, 0};
            vk_transfer_queue_family = {*transfer_queue_family_index, 0};

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
            vk_graphics_queue_family = {*graphics_queue_family_index, 0};
            vk_transfer_queue_family = {*graphics_queue_family_index, 1};

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
            vk_graphics_queue_family = {*graphics_queue_family_index, 0};
            vk_transfer_queue_family = {*graphics_queue_family_index, 0};

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

        device = this->instance->CreateDevice(vk_physical_device, info);

        auto const vma_functions = context.GetVmaFunctions(instance, *device);

        VmaAllocatorCreateInfo allocator_info {};
        allocator_info.instance = this->instance->Handle();
        allocator_info.device = device->Handle();
        allocator_info.physicalDevice = this->vk_physical_device;
        allocator_info.vulkanApiVersion = VK_API_VERSION_1_4; //------------------------------- Precist z instance
        allocator_info.pVulkanFunctions = &vma_functions;
        allocator_info.flags = allocator_ext_flags;

        if (auto vk_result = vmaCreateAllocator(&allocator_info, &vma_allocator); vk_result != VK_SUCCESS)
        {
            throw VulkanException(vk_result);
        }
    }

    std::map<std::string, VulkanVersion> Device::EnumerateExtensions() const
    {
        std::map<std::string, VulkanVersion> result;

        auto const count = instance->EnumerateDeviceExtensionPropertiesCount(vk_physical_device, {});

        std::vector<VkExtensionProperties> properties(count);

        for (auto const& extension : instance->EnumerateDeviceExtensionProperties(vk_physical_device, {}, properties))
        {
            result[extension.extensionName] = {extension.specVersion};
        }

        return result;
    }

    std::vector<QueueFamilyProperties> Device::GetQueueFamilyProperties(Surface const& surface) const
    {
        auto const count = instance->GetPhysicalDeviceQueueFamilyPropertiesCount(vk_physical_device);
        std::vector<VkQueueFamilyProperties> properties(count);
        instance->GetPhysicalDeviceQueueFamilyProperties(vk_physical_device, properties);

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
                .presentation = instance->GetPhysicalDevicePresentationSupport(vk_physical_device, index),
                .surface = instance->GetPhysicalDeviceSurfaceSupportKHR(vk_physical_device, index, surface.Handle())
            });
        }

        return result;
    }

    Device::~Device()
    {
        WaitIdle();

        if (vma_allocator != VK_NULL_HANDLE)
        {
            vmaDestroyAllocator(vma_allocator);
        }
    }

    std::unique_ptr<SwapChain> Device::CreateSwapChain(Surface const& surface, Window const& window)
    {
        return std::make_unique<SwapChain>(*device, surface.vk_surface, window);
    }

    std::unique_ptr<Shader> Device::CreateShader(std::span<uint32_t const> spirv)
    {
        return std::make_unique<Shader>(*device, spirv);
    }

    std::unique_ptr<RenderCommandQueue> Device::CreateGraphicsQueue() const
    {
        return std::make_unique<RenderCommandQueue>(*device, vk_graphics_queue_family.first, vk_graphics_queue_family.second);
    }

    std::unique_ptr<TransferCommandQueue> Device::CreateTransferQueue() const
    {
        return std::make_unique<TransferCommandQueue>(*device, vk_transfer_queue_family.first, vk_transfer_queue_family.second);
    }

    std::unique_ptr<Fence> Device::CreateFence() const
    {
        return std::make_unique<Fence>(*device);
    }

    std::unique_ptr<Semaphore> Device::CreateSemaphore() const
    {
        return std::make_unique<Semaphore>(*device);
    }

    std::unique_ptr<TimelineSemaphore> Device::CreateTimelineSemaphore() const
    {
        return std::make_unique<TimelineSemaphore>(*device);
    }

    void Device::WaitIdle() const noexcept
    {
        if (device)
        {
            device->WaitIdle();
        }
    }

    std::shared_ptr<PipelineLayout> Device::CreatePipelineLayout()
    {
        return std::make_shared<PipelineLayout>(*device);
    }

    PipelineFactory Device::CreatePipelineFactory()
    {
        return PipelineFactory(*device);
    }

    std::unique_ptr<ResourceDescriptorHeap> Device::CreateResourceDescriptorHeap(std::span<ResourceDescriptor const> descriptors) const
    {
        return std::make_unique<ResourceDescriptorHeap>(*instance, *device, vk_physical_device, descriptors);
    }

    std::unique_ptr<Buffer> Device::AllocateVertexBuffer(uint64_t size) const
    {
        return std::make_unique<Buffer>(
            *device,
            size,
            vma_allocator,
            VkBufferUsageFlags2
            {
                VK_BUFFER_USAGE_2_TRANSFER_DST_BIT |
                VK_BUFFER_USAGE_2_VERTEX_BUFFER_BIT
            },
            VmaAllocationCreateFlags{}
        );
    }

    std::unique_ptr<Buffer> Device::AllocateIndexBuffer(uint64_t size) const
    {
        return std::make_unique<Buffer>(
            *device,
            size,
            vma_allocator,
            VkBufferUsageFlags2
            {
                VK_BUFFER_USAGE_2_TRANSFER_DST_BIT |
                VK_BUFFER_USAGE_2_INDEX_BUFFER_BIT
            },
            VmaAllocationCreateFlags{}
        );
    }

    std::unique_ptr<Buffer> Device::AllocateStagingBuffer(uint64_t size) const
    {
        return std::make_unique<Buffer>(
            *device,
            size,
            vma_allocator,
            VkBufferUsageFlags2
            {
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT
            },
            VmaAllocationCreateFlags
            {
                VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                VMA_ALLOCATION_CREATE_MAPPED_BIT
            }
        );
    }

    std::unique_ptr<Buffer> Device::AllocateUniformBuffer(uint64_t size) const
    {
        return std::make_unique<Buffer>(
            *device,
            size,
            vma_allocator,
            VkBufferUsageFlags2
            {
                VK_BUFFER_USAGE_2_UNIFORM_BUFFER_BIT |
                VK_BUFFER_USAGE_2_SHADER_DEVICE_ADDRESS_BIT
            },
            VmaAllocationCreateFlags
            {
                VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                VMA_ALLOCATION_CREATE_MAPPED_BIT
            }
        );
    }

    std::unique_ptr<Buffer> Device::AllocateDescriptorHeapBuffer(uint64_t size) const
    {
        return std::make_unique<Buffer>(
            *device,
            size,
            vma_allocator,
            VkBufferUsageFlags2
            {
                VK_BUFFER_USAGE_DESCRIPTOR_HEAP_BIT_EXT |
                VK_BUFFER_USAGE_2_TRANSFER_DST_BIT |
                VK_BUFFER_USAGE_2_SHADER_DEVICE_ADDRESS_BIT
            },
            VmaAllocationCreateFlags{}
        );
    }

    std::unique_ptr<Texture2D> Device::AllocateDepthBuffer(uint32_t width, uint32_t height) const
    {
        return std::make_unique<Texture2D>(
            *device,
            vma_allocator,
            PixelFormat::DepthBuffer,
            width,
            height,
            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            VK_IMAGE_LAYOUT_UNDEFINED
        );
    }

} // Rc::Render