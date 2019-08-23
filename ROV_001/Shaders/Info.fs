/*
 * ROV_001 
 * Copyright (C) 2019 ROV Simulator
 * National Taiwan Ocean University 
 * Department of Computer Science and Engineering
 * Lab 503 Chieh-Shih Chou, Shyh-Kuang Ueng
 * Copyright (C) 2019
 */
// Fragment shader:
// ================
#version 430 core

uniform vec3 LineColor;

out vec4 FragColor;

void main()
{
    FragColor = vec4(LineColor, 1.0);
}