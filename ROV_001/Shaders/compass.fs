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

in vec2 TexCoords;

uniform sampler2D Tex1;

out vec4 FragColor;

void main()
{    
    vec4 texColor = texture(Tex1, TexCoords.rg);

    if(texColor.a <= 0.1) // Or whichever comparison here
        discard;

    FragColor = texColor;
}

