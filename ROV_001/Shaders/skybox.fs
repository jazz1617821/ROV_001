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

in vec3 TexCoords;
in float depth;

uniform samplerCube skybox;

vec4 depthFoggy(vec4 Color);

void main()
{    
    FragColor = texture(skybox, TexCoords);
}
