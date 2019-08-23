#version 430 core

layout (local_size_x = 16, local_size_y = 16) in;

// 傳遞卷積核
uniform float v[4];

layout (rgba32f, binding = 0) uniform image2D input_image;
layout (rgba32f, binding = 1) uniform image2D output_image;

shared vec4 scanline[16][16];

void main(void)
{
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    scanline[pos.x][pos.y] = imageLoad(input_image, pos);
    barrier();
    vec4 data = scanline[pos.x][pos.y];
    data.r = v[0] + data.r;
    data.g = v[1] + data.g;
    data.b = v[2] + data.b;
    data.a = v[3] + data.a;
    imageStore(output_image, pos.xy, data);
}