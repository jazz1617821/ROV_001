#version 430 core
layout (location = 0) in vec3 position;
layout ( std140 ) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

void main()
{
    vec4 pos = projection * view * vec4(position, 1.0f);
    pos.z = -1.0;
    gl_Position = pos;
}