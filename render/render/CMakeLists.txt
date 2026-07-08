find_package(Vulkan REQUIRED)

add_library(render STATIC
    "attachment.h"
    "buffer.h"
    "buffer.cpp"
    "buffer_linear_allocator.h"
    "buffer_ring_allocator.h"
    "buffer_ring_allocator.cpp"
    "buffer_pool_allocator.h"
    "constants.h"
    "instance.h"
    "instance.cpp"
    "device.h"
    "device.cpp"
    "error.h"
    "adapter.h"
    "adapter.cpp"
    "renderer.h"
    "renderer.cpp"
    "frame_renderer.h"
    "frame_renderer.cpp"
    "swap_chain.h"
    "swap_chain.cpp"
    "pipeline_state.h"
    "pipeline_state.cpp"
    "command_queue.h"
    "command_queue.cpp"
    "command_buffer.h"
    "command_buffer.cpp"
    "fence.h"
    "fence.cpp"
    "image.h" ######################x delete
    "image.cpp"
    "shader.h"
    "shader.cpp"
    "texture.h"
    "texture.cpp"
    "vertex_traits.h"
    "index_buffer.h"
    "index_buffer.cpp"
    "surface.h"
    "surface.cpp"
    "semaphore.h"
    "semaphore.cpp"
    "version.h"
    "descriptor_heap.h"
    "descriptor_heap.cpp"
)

target_link_libraries(render
    PUBLIC vulkan
    PUBLIC platform
    PUBLIC resources
    PUBLIC core
)

target_compile_features(render
    PUBLIC cxx_std_26
)

target_compile_definitions(render
    PUBLIC VK_USE_PLATFORM_WIN32_KHR=1
    PUBLIC VMA_DYNAMIC_VULKAN_FUNCTIONS=1
    PUBLIC VMA_STATIC_VULKAN_FUNCTIONS=0
)
