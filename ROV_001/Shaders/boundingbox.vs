#version 430 core
layout (location = 0) in vec4 position;
layout ( std140 ) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

void main()
{
    gl_Position = projection * view * position;
}