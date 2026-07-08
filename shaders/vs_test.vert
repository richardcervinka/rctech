#version 460
#extension GL_EXT_descriptor_heap : require
 
layout(descriptor_heap) uniform Constants
{
    mat4 camera_projection;
    mat4 transformation; // ---------------- Bude soucasti per-instance dat
} ubo[];

// Input
layout(location = 0) in vec3 in_position;
layout(location = 2) in vec3 in_color;
layout(location = 3) in vec4 in_transformation_c0;
layout(location = 4) in vec4 in_transformation_c1;
layout(location = 5) in vec4 in_transformation_c2;
layout(location = 6) in vec4 in_transformation_c3;

// Output
layout(location = 0) out vec3 out_color;

void main()
{
    vec4 position = vec4(in_position, 1.0);

    mat4x4 transformations = mat4x4(
        in_transformation_c0,
        in_transformation_c1,
        in_transformation_c2,
        in_transformation_c3
    );

    position = transformations * position;
    position = ubo[0].camera_projection * position;

    gl_Position = position;
    out_color = in_color;
}
