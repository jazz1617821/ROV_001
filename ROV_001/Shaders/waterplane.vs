/*
 * ROV_001 
 * Copyright (C) 2019 ROV Simulator
 * National Taiwan Ocean University 
 * Department of Computer Science and Engineering
 * Lab 503 Chieh-Shih Chou, Shyh-Kuang Ueng
 * Copyright (C) 2019
 */
#version 430 core

#extension GL_NV_shader_buffer_load : enable

#define PI 3.1415926
#define MAX_SIN_WAVE 10

layout (location = 0) in vec2 pos;
layout ( std140 ) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

struct SinWave {
    float wavelength;
    float amplitude;
    float speed;
    vec2 direction;
}; 

uniform SinWave wave[MAX_SIN_WAVE];
uniform int num_wave;
uniform float time;

float CalculateHight(SinWave wave, vec2 pos, float time);

void main()
{
    float high = 0;
    for(int i = 0; i < num_wave; i++)
    {
        high += CalculateHight(wave[i], pos, time);
    }
    vec3 pos_with_high = vec3(pos.x, high, pos.y);

    gl_Position = projection * view * vec4( pos_with_high, 1.0 );
}


float CalculateHight( SinWave wave, vec2 pos, float time)
{
    float high = 0;
    float lambda = 2 * PI / wave.wavelength;
    float phase_velocity = wave.speed * 2 * PI / wave.wavelength;
    high = wave.amplitude * sin( dot(wave.direction, pos) * lambda + time * phase_velocity);

    return high;
}