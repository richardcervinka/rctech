#version 460
#extension GL_EXT_descriptor_heap : require
#extension GL_EXT_nonuniform_qualifier : require

// View to the descriptor heap.
layout(descriptor_heap) uniform Constants
{
    mat4 camera_projection;
}
ubo[];

layout(push_constant) uniform PushData
{
    uint ubo_index;
}
push_data;

// Input
layout(location = 0) in vec3 in_position;
// Local transformation matrix
layout(location = 3) in vec4 in_local_0;
layout(location = 4) in vec4 in_local_1;
layout(location = 5) in vec4 in_local_2;
layout(location = 6) in vec4 in_local_3;
// Global transformation matrix
layout(location = 7) in vec4 in_world_0;
layout(location = 8) in vec4 in_world_1;
layout(location = 9) in vec4 in_world_2;
layout(location = 10) in vec4 in_world_3;
// Texture UV
layout(location = 11) in vec2 in_uv;

// Output
layout(location = 0) out vec3 out_color;
layout(location = 1) out vec2 out_uv;

void main()
{
    vec4 position = vec4(in_position, 1.0);

    // Local transformations
    position = mat4x4(in_local_0, in_local_1, in_local_2, in_local_3) * position;
    // Global transformations
    position = mat4x4(in_world_0, in_world_1, in_world_2, in_world_3) * position;
    // Camera projection
    position = ubo[push_data.ubo_index].camera_projection * position; // push_data.ubo_index

    gl_Position = position;

    out_color = vec3(in_uv.r, in_uv.g, 0);

    out_uv = in_uv;
}
