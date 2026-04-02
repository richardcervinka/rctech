find_package(Vulkan REQUIRED)

add_library(renderer STATIC
    # "buffer.h"
    # "buffer.cpp"
    "instance.h"
    "instance.cpp"
    "device.h"
    "device.cpp"
    "error.h"
    "adapter.h"
    "adapter.cpp"
    "renderer.h"
    "renderer.cpp"
    "swap_chain.h"
    "swap_chain.cpp"
    "pipeline_state.h"
    "pipeline_state.cpp"
    "command_queue.h"
    "command_queue.cpp"
    "command_list.h"
    "command_list.cpp"
    "fence.h"
    "fence.cpp"
    "image.h" ######################x delete
    "image.cpp"
    "shader.h"
    "shader.cpp"
    "texture.h"
    "texture.cpp"
    "vertex_traits.h"
    "vertex_buffer.h"
    "vertex_buffer.cpp"
    "staging_buffer.h"
    "staging_buffer.cpp"
    "surface.h"
    "surface.cpp"
    "semaphore.h"
    "semaphore.cpp"
)

target_link_libraries(renderer
    PUBLIC vulkan
    PUBLIC platform
    PUBLIC resources
    PUBLIC core
)

target_compile_features(renderer
    PUBLIC cxx_std_26
)

target_compile_definitions(renderer
    PUBLIC VK_USE_PLATFORM_WIN32_KHR=1
    PUBLIC VMA_DYNAMIC_VULKAN_FUNCTIONS=1
    PUBLIC VMA_STATIC_VULKAN_FUNCTIONS=0
)
