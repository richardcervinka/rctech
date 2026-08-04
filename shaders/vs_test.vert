#version 460
#extension GL_EXT_descriptor_heap : require

// View to the descriptor heap.
layout(descriptor_heap) uniform Constants
{
    mat4 camera_projection;
    mat4 transformation; // ---------------- Bude soucasti per-instance dat
} ubo[];

// Input
layout(location = 0) in vec3 in_position;
layout(location = 2) in vec3 in_color;
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

// Output
layout(location = 0) out vec3 out_color;

void main()
{
    vec4 position = vec4(in_position, 1.0);

    position = mat4x4(in_local_0, in_local_1, in_local_2, in_local_3) * position;
    position = mat4x4(in_world_0, in_world_1, in_world_2, in_world_3) * position;
    position = ubo[0].camera_projection * position;

    gl_Position = position;
    out_color = in_color;
}
