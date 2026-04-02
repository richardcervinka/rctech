find_package(Vulkan REQUIRED)

add_library(vulkan STATIC
    "allocator.h"
    "allocator.cpp"
    "error.h"
    "context.h"
    "context.cpp"
    "instance.h"
    "instance.cpp"
    "device.h"
    "device.cpp"
)

target_link_libraries(vulkan
    PUBLIC Vulkan::Vulkan
    PUBLIC platform
    PUBLIC GPUOpen::VulkanMemoryAllocator
)

target_compile_features(vulkan
    PUBLIC cxx_std_26
)

target_compile_definitions(vulkan
    PUBLIC VK_USE_PLATFORM_WIN32_KHR=1
    PUBLIC VMA_DYNAMIC_VULKAN_FUNCTIONS=1
    PUBLIC VMA_STATIC_VULKAN_FUNCTIONS=0
)
