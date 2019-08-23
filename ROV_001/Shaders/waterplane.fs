/*
 * ROV_001 
 * Copyright (C) 2019 ROV Simulator
 * National Taiwan Ocean University 
 * Department of Computer Science and Engineering
 * Lab 503 Chieh-Shih Chou, Shyh-Kuang Ueng
 * Copyright (C) 2019
 */
#version 430 core

out vec4 FragColor;

void main()
{
    vec3 color = vec3(0.098f, 0.34f, 0.525f) * 1.6;
    FragColor = vec4(color, 0.1);
}