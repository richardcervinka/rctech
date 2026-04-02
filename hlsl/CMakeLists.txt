# Compile HLSL to SPIR-V
function(target_compile_hlsl CUSTOM_TARGET FILE_NAME SHADER)
    get_filename_component(FILE_WE "${FILE_NAME}" NAME_WE)

    add_custom_command(
        OUTPUT "${FILE_WE}.spv"
        COMMAND ${TOOL_DXC} -T ${SHADER} -E main -spirv -Fo "${FILE_WE}.spv" "${CMAKE_CURRENT_SOURCE_DIR}/${FILE_NAME}"
        DEPENDS "${FILE_NAME}"
    )

    add_custom_target("${FILE_WE}_spv" DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/${FILE_WE}.spv")
    add_dependencies(${CUSTOM_TARGET} "${FILE_WE}_spv")
endfunction()

add_custom_target(spirv ALL)

target_compile_hlsl(spirv "vs_dummy.hlsl" vs_6_0)
target_compile_hlsl(spirv "vs_overlay.hlsl" vs_6_0)
target_compile_hlsl(spirv "vs_test.hlsl" vs_6_0)
target_compile_hlsl(spirv "ps_dummy.hlsl" ps_6_0)
