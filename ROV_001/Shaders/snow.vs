/*
 * ROV_001 
 * Copyright (C) 2019 ROV Simulator
 * National Taiwan Ocean University 
 * Department of Computer Science and Engineering
 * Lab 503 Chieh-Shih Chou, Shyh-Kuang Ueng
 * Copyright (C) 2019
 */
#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 diffColor;
layout (location = 4) in vec3 specColor;
layout (location = 5) in vec3 ambiColor;
layout ( std140 ) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

out vec2 TexCoords;

uniform mat4 model;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
    TexCoords = texCoords;	//assimp's aiProcess_FlipUVs will adjust
}