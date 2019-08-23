/*
 * ROV_001
 * Copyright (C) 2019 ROV Simulator
 * National Taiwan Ocean University
 * Department of Computer Science and Engineering
 * Computer Graphics Lab Chieh-Shih Chou, Shyh-Kuang Ueng
 * Copyright (C) 2019
 */
#include "MyWaterPlane.hpp"

MyWaterPlane::MyWaterPlane(int length = 100, int width = 100, float scl = 1.0)
{
	this->width = width;
	this->height = length;
	this->scl = scl;
	this->cols = (this->width / this->scl) + 1;
	this->rows = (this->height / this->scl) + 1;
	this->num_sinwave = 0;

	// gen elemental list
	this->indices = new GLuint[6 * (this->cols - 1) * (this->rows - 1)];
	for (int y = 0; y < this->rows - 1; y++) {
		for (int x = 0; x < this->cols - 1; x++) {
			indices[y * (this->cols - 1) * 6 + x * 6 + 0] = y * this->cols + x;
			indices[y * (this->cols - 1) * 6 + x * 6 + 1] = (y + 1) * this->cols + x;
			indices[y * (this->cols - 1) * 6 + x * 6 + 2] = y * this->cols + (x + 1);
			indices[y * (this->cols - 1) * 6 + x * 6 + 3] = (y + 1) * this->cols + x;
			indices[y * (this->cols - 1) * 6 + x * 6 + 4] = (y + 1) * this->cols + (x + 1);
			indices[y * (this->cols - 1) * 6 + x * 6 + 5] = y * this->cols + (x + 1);
		}
	}
}

MyWaterPlane::~MyWaterPlane()
{
}

void MyWaterPlane::genWaterPlane(Shader shader)
{
	// gen vertex
	glm::vec2* vertexs = new glm::vec2[this->cols * this->rows];
	float x_offset = -width / 2;
	float z_offset = -height / 2;

	for (int y = 0; y < this->rows; y++) {
		for (int x = 0; x < this->cols; x++) {
			vertexs[y * this->cols + x].x = x_offset + x * this->scl;
			vertexs[y * this->cols + x].y = z_offset + y * this->scl;
		}
	}

	GLuint vertex_vbo;
	GLuint indices_ebo;
	shader.use();
	// Create VBO with point coordinates
	glGenBuffers(1, &vertex_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * this->cols * this->rows, vertexs, GL_STATIC_DRAW);


	glGenBuffers(1, &indices_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * 6 * (this->cols - 1) * (this->rows - 1), this->indices, GL_STATIC_DRAW);

	// Create VAO
	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);
	// Specify the layout of the vertex data
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_vbo);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), 0);
	glBindVertexArray(0);
}

void MyWaterPlane::genSinWave(float wavelength, float amplitude, float speed, glm::vec2 direction)
{
	this->sinwave[this->num_sinwave].wavelength = wavelength;
	this->sinwave[this->num_sinwave].amplitude = amplitude;
	this->sinwave[this->num_sinwave].speed = speed;
	this->sinwave[this->num_sinwave].direction = direction;
	this->num_sinwave++;
}

void MyWaterPlane::populateSinWaveParameter(Shader shader)
{
	shader.use();
	for (int i = 0; i < this->num_sinwave; i++)
	{
		shader.setFloat("wave[" + std::to_string(i) + "].wavelength", this->sinwave[i].wavelength);
		shader.setFloat("wave[" + std::to_string(i) + "].amplitude", this->sinwave[i].amplitude);
		shader.setFloat("wave[" + std::to_string(i) + "].speed", this->sinwave[i].speed);
		shader.setVec2("wave[" + std::to_string(i) + "].direction", this->sinwave[i].direction);
	}
	shader.setInt("num_wave", this->num_sinwave);
}

void MyWaterPlane::draw(Shader shader)
{
	shader.use();
	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES, 6 * (this->cols - 1) * (this->rows - 1), GL_UNSIGNED_INT, this->indices);
}

