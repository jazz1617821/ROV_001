/*
 * ROV_001 
 * Copyright (C) 2019 ROV Simulator
 * National Taiwan Ocean University 
 * Department of Computer Science and Engineering
 * Lab 503 Chieh-Shih Chou, Shyh-Kuang Ueng
 * Copyright (C) 2019
 */
#version 330 core

in vec2 TexCoords;

out vec4 color;

uniform sampler2D texture1;

void main()
{    
    vec4 texColor = texture(texture1, TexCoords);
    if(texColor.a < 0.5)
        discard;
    color = texColor;
}