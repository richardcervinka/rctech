#version 460
#extension GL_EXT_descriptor_heap : require
 
layout(descriptor_heap) uniform Constants
{
    mat4 transformation;
    mat4 camera_projection;
} ubo[];

// Input
layout(location = 0) in vec3 in_position;
layout(location = 2) in vec3 in_color;

// Output
layout(location = 0) out vec3 out_color;

void main()
{
    vec4 position = vec4(in_position, 1.0);

    position = ubo[0].transformation * position;
    position = ubo[0].camera_projection * position;

    gl_Position = position;
    out_color = in_color;
}
