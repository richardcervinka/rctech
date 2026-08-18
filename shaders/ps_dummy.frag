#version 460
#extension GL_EXT_descriptor_heap : require
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_samplerless_texture_functions : require // for texelFetch

// View to the descriptor heap.
layout(descriptor_heap) uniform texture2D textures[];

layout(location = 0) in vec3 in_color;
layout(location = 1) in vec2 in_uv;

layout(location = 0) out vec4 out_color;

void main()
{
    ivec2 texCoord = ivec2(in_uv.x * 16, in_uv.y * 16);
    out_color = texelFetch(textures[4], texCoord, 0);
    //out_color = vec4(in_color, 0);
}