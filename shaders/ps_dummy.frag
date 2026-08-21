#version 460
#extension GL_EXT_descriptor_heap : require
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_samplerless_texture_functions : require // for texelFetch

// Views to the descriptor heap.
layout(descriptor_heap) uniform texture2D textures[];
layout(descriptor_heap) uniform sampler samplers[];

// Input
layout(location = 0) in vec3 in_color;
layout(location = 1) in vec2 in_uv;

// Output
layout(location = 0) out vec4 out_color;

void main()
{
    // out_color = texelFetch(textures[4], texCoord, 0);
    out_color =  texture(sampler2D(textures[4], samplers[0]), in_uv);
}