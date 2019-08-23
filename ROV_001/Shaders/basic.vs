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
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 Diff;
layout (location = 4) in vec3 Spec;
layout (location = 5) in vec3 Ambi;
layout ( std140 ) uniform Matrices
{
    mat4 projection;
    mat4 view;
};
uniform mat4 model;

out vec2 TexCoords;
out vec3 fragPosition;
out vec3 Normal;

out vec3 fragAmbient;
out vec3 fragDiffuse;
out vec3 fragSpecular;

void main()
{
    fragAmbient = Ambi;
    fragDiffuse = Diff;
    fragSpecular = Spec;

    gl_Position = projection * view * model * vec4(position, 1.0f);
    fragPosition = vec3(model * vec4(position, 1.0f));
    Normal = mat3(transpose(inverse(model))) * normal;
    TexCoords = texCoords;
}