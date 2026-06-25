# Compile HLSL to SPIR-V

find_package(Vulkan REQUIRED)

find_program(
    EXE_GLSLANG_VALIDATOR glslangValidator
    PATHS "${Vulkan_INCLUDE_DIRS}/../Bin"
    NO_DEFAULT_PATH
    REQUIRED
)

find_program(
    TOOL_DXC dxc
    PATHS "${Vulkan_INCLUDE_DIRS}/../Bin"
    NO_DEFAULT_PATH
    REQUIRED
)

function(target_compile_hlsl CUSTOM_TARGET FILE_NAME SHADER)
    get_filename_component(FILE_WE "${FILE_NAME}" NAME_WE)

    add_custom_command(
        OUTPUT "${FILE_WE}.spv"
        COMMAND ${TOOL_DXC} -T ${SHADER} -E main -spirv -fspv-target-env=vulkan1.3 -fspv-extension=SPV_EXT_descriptor_heap -fspv-extension=SPV_KHR_untyped_pointers -fspv-use-descriptor-heap -Fo "${FILE_WE}.spv" "${CMAKE_CURRENT_SOURCE_DIR}/${FILE_NAME}"
        DEPENDS "${FILE_NAME}"
    )

    add_custom_target("${FILE_WE}_spv" DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}.spv")
    add_dependencies(${CUSTOM_TARGET} "${FILE_WE}_spv")
endfunction()

function(target_compile_glsl CUSTOM_TARGET FILE_NAME)
    get_filename_component(FILE_WE "${FILE_NAME}" NAME_WE)

    add_custom_command(
        OUTPUT "${FILE_WE}.spv"
        COMMAND ${EXE_GLSLANG_VALIDATOR} "${CMAKE_CURRENT_SOURCE_DIR}/${FILE_NAME}" --target-env vulkan1.4 -o "${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}.spv"
        DEPENDS "${FILE_NAME}"
    )

    add_custom_target("${FILE_WE}_spv" DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}.spv")
    add_dependencies(${CUSTOM_TARGET} "${FILE_WE}_spv")
endfunction()

add_custom_target(spirv ALL)

target_compile_hlsl(spirv "vs_dummy.hlsl" vs_6_6)
target_compile_hlsl(spirv "vs_overlay.hlsl" vs_6_6)
target_compile_glsl(spirv "ps_dummy.frag" ps_6_6)
target_compile_glsl(spirv "vs_test.vert")

