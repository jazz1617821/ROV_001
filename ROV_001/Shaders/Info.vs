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

uniform vec2 center_point;
uniform float X_width;
uniform float Y_width;

void main()
{
    gl_Position = vec4((position.x - center_point.x) / X_width, (position.y - center_point.y) / Y_width, 0.0f, 1.0f);
}