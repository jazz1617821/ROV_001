/*
 * ROV_001 
 * Copyright (C) 2019 ROV Simulator
 * National Taiwan Ocean University 
 * Department of Computer Science and Engineering
 * Lab 503 Chieh-Shih Chou, Shyh-Kuang Ueng
 * Copyright (C) 2019
 */
#version 430 core
layout (location = 0) in vec3 aPos;
layout ( std140 ) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

out vec3 TexCoords;

void main()
{
    mat4 viewFix = mat4(mat3(view));
    TexCoords = aPos;
    vec4 pos = projection * viewFix * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}  