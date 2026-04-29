# Vulkan Memory Allocator

add_subdirectory("VulkanMemoryAllocator")

# Tiny glTF

set(TINYGLTF_HEADER_ONLY ON CACHE INTERNAL "" FORCE)
set(TINYGLTF_INSTALL OFF CACHE INTERNAL "" FORCE)
set(TINYGLTF_BUILD_LOADER_EXAMPLE OFF CACHE INTERNAL "" FORCE)

add_subdirectory("tinygltf")

target_compile_definitions(tinygltf
    INTERFACE TINYGLTF_NO_STB_IMAGE=1
    INTERFACE TINYGLTF_NO_STB_IMAGE_WRITE=1
    INTERFACE TINYGLTF_NO_INCLUDE_STB_IMAGE=1
    INTERFACE TINYGLTF_USE_RAPIDJSON=1
)

# googletest

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(BUILD_GMOCK ON CACHE BOOL "Enable/Disable gmock" FORCE)

add_subdirectory("googletest")