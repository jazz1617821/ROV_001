/*
 * ROV_001 
 * Copyright (C) 2019 ROV Simulator
 * National Taiwan Ocean University 
 * Department of Computer Science and Engineering
 * Lab 503 Chieh-Shih Chou, Shyh-Kuang Ueng
 * Copyright (C) 2019
 */
// Vertex shader:
// ================
#version 430 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texCoords;

uniform float depth;
uniform float width;
uniform float height;
uniform mat4 model;

out vec2 TexCoords;

void main()
{
    TexCoords = texCoords;
    vec4 newPosition = model * vec4(position,0.0,1.0);
    gl_Position = vec4(newPosition.x / width * 2 - 1, newPosition.y / height * 2 - 1, depth, 1.0);
}