/*
 * ROV_001
 * Copyright (C) 2019 ROV Simulator
 * National Taiwan Ocean University
 * Department of Computer Science and Engineering
 * Computer Graphics Lab Chieh-Shih Chou, Shyh-Kuang Ueng
 * Copyright (C) 2019
 */
#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Shader.hpp"
#include <iostream>

#define MAX_SIN_WAVE 10

struct SinWave {
	float wavelength;
	float amplitude;
	float speed;
	glm::vec2 direction;
};

class MyWaterPlane
{
public:
	MyWaterPlane(int length, int width, float scl);
	~MyWaterPlane();

	void genWaterPlane(Shader shader);
	void genSinWave(float wavelength, float amplitude, float speed, glm::vec2 direction);
	void populateSinWaveParameter(Shader shader);
	void draw(Shader shader);

private:
	int num_sinwave;
	SinWave sinwave[MAX_SIN_WAVE];
	GLuint VAO;
	GLuint* indices;
	int width;
	int height;
	float scl;
	int cols;
	int rows;
};

