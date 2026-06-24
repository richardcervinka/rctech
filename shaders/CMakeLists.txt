# Compile HLSL to SPIR-V
find_package(Vulkan REQUIRED)


find_program(
    TOOL_GLSLC glslangValidator
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

    # -fspv-extension=SPV_EXT_descriptor_heap
    # -fspv-extension=SPV_KHR_untyped_pointers
    # -fspv-extension=SPV_EXT_descriptor_indexing
    add_custom_command(
        OUTPUT "${FILE_WE}.spv"
        COMMAND ${TOOL_DXC} -T ${SHADER} -E main -spirv -fspv-target-env=vulkan1.3 -fspv-extension=SPV_EXT_descriptor_heap -fspv-extension=SPV_KHR_untyped_pointers -fspv-use-descriptor-heap -Fo "${FILE_WE}.spv" "${CMAKE_CURRENT_SOURCE_DIR}/${FILE_NAME}"
        DEPENDS "${FILE_NAME}"
    )

    add_custom_target("${FILE_WE}_spv" DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}.spv")
    add_dependencies(${CUSTOM_TARGET} "${FILE_WE}_spv")
endfunction()

function(target_compile_slang CUSTOM_TARGET FILE_NAME)
    get_filename_component(FILE_WE "${FILE_NAME}" NAME_WE)
    set(TOOL_SLANG "C:/Program Files/Slang/2026.11/bin/slangc.exe")

    # -fspv-extension=SPV_EXT_descriptor_heap
    # -fspv-extension=SPV_KHR_untyped_pointers
    # -fspv-extension=SPV_EXT_descriptor_indexing
    add_custom_command(
        OUTPUT "${FILE_WE}.spv"
        COMMAND ${TOOL_SLANG} -target spirv -profile spirv_1_6 -entry main -stage vertex -capability SPV_EXT_descriptor_heap -o "${FILE_WE}.spv" "${CMAKE_CURRENT_SOURCE_DIR}/${FILE_NAME}"
        DEPENDS "${FILE_NAME}"
    )

    add_custom_target("${FILE_WE}_spv" DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}.spv")
    add_dependencies(${CUSTOM_TARGET} "${FILE_WE}_spv")
endfunction()

function(target_compile_glsl CUSTOM_TARGET FILE_NAME SHADER)
    get_filename_component(FILE_WE "${FILE_NAME}" NAME_WE)

    # glslc shader.vert -o shader.vert.spv
    # -fspv-extension=SPV_EXT_descriptor_heap
    # -fspv-extension=SPV_KHR_untyped_pointers
    # -fspv-extension=SPV_EXT_descriptor_indexing
    add_custom_command(
        OUTPUT "${FILE_WE}.spv"
        COMMAND ${TOOL_GLSLC} "${CMAKE_CURRENT_SOURCE_DIR}/${FILE_NAME}" --target-env=vulkan1.4 -o "${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}.spv"
        DEPENDS "${FILE_NAME}"
    )

    add_custom_target("${FILE_WE}_spv" DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}.spv")
    add_dependencies(${CUSTOM_TARGET} "${FILE_WE}_spv")
endfunction()

add_custom_target(spirv ALL)


target_compile_hlsl(spirv "vs_dummy.hlsl" vs_6_6)
target_compile_hlsl(spirv "vs_overlay.hlsl" vs_6_6)
#target_compile_hlsl(spirv "vs_test.hlsl" vs_6_6)
target_compile_glsl(spirv "ps_dummy.frag" ps_6_6)
target_compile_glsl(spirv "vs_test.vert" vs_6_6)


# slangc shader.slang -target spirv -profile spirv_1_6 -capability SPV_EXT_descriptor_heap -o shader.spv
