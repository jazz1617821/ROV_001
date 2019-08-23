/*
 * ROV_001
 * Copyright (C) 2019 ROV Simulator
 * National Taiwan Ocean University
 * Department of Computer Science and Engineering
 * Lab 503 Chieh-Shih Chou, Shyh-Kuang Ueng
 * Copyright (C) 2019
 */
#pragma once
 // GL include
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/norm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Shader.hpp"


class AABB {
public:
	// method
	AABB(glm::vec3 min = glm::vec3(0.0f), glm::vec3 max = glm::vec3(0.0f))
	{
		this->min = glm::vec4(min, 1.0);
		this->max = glm::vec4(max, 1.0);
	}
	~AABB()
	{
	}
	AABB projectionTransfer(glm::mat4 projectionMatrix)
	{
		glm::vec3 min(FLT_MAX, FLT_MAX, FLT_MAX), max(-FLT_MAX, -FLT_MAX, -FLT_MAX);
		glm::vec4 _8_point_inWCS[8];
		this->calculate_8_point_inWCS(_8_point_inWCS);
		for (int i = 0; i < 8; i++) {
			glm::vec3 point_after_transform = projectionMatrix * _8_point_inWCS[i];
			for (int j = 0; j < 3; j++) {
				if (point_after_transform[j] < min[j]) {
					min[j] = point_after_transform[j];
				}
				if (point_after_transform[j] > max[j]) {
					max[j] = point_after_transform[j];
				}
			}
		}
		return AABB(min, max);
	}
	void calculate_8_point_inWCS(glm::vec4* eight_point_inWCS)
	{ 
		eight_point_inWCS[0] = glm::vec4(this->min.x, this->min.y, this->min.z, 1.0);
		eight_point_inWCS[1] = glm::vec4(this->min.x, this->min.y, this->max.z, 1.0);
		eight_point_inWCS[2] = glm::vec4(this->max.x, this->min.y, this->max.z, 1.0);
		eight_point_inWCS[3] = glm::vec4(this->max.x, this->min.y, this->min.z, 1.0);
		eight_point_inWCS[4] = glm::vec4(this->min.x, this->max.y, this->min.z, 1.0);
		eight_point_inWCS[5] = glm::vec4(this->min.x, this->max.y, this->max.z, 1.0);
		eight_point_inWCS[6] = glm::vec4(this->max.x, this->max.y, this->max.z, 1.0);
		eight_point_inWCS[7] = glm::vec4(this->max.x, this->max.y, this->min.z, 1.0);
	}
	void drawBoundingBox(Shader * shader)
	{
		glm::vec4 _8_point_inWCS[8];
		this->calculate_8_point_inWCS(_8_point_inWCS);

		glm::vec4 boundingboxvertices[24] = {
			glm::vec4(_8_point_inWCS[0]),
			glm::vec4(_8_point_inWCS[1]),
			glm::vec4(_8_point_inWCS[1]),
			glm::vec4(_8_point_inWCS[2]),
			glm::vec4(_8_point_inWCS[2]),
			glm::vec4(_8_point_inWCS[3]),
			glm::vec4(_8_point_inWCS[3]),
			glm::vec4(_8_point_inWCS[0]),
			glm::vec4(_8_point_inWCS[4]),
			glm::vec4(_8_point_inWCS[5]),
			glm::vec4(_8_point_inWCS[5]),
			glm::vec4(_8_point_inWCS[6]),
			glm::vec4(_8_point_inWCS[6]),
			glm::vec4(_8_point_inWCS[7]),
			glm::vec4(_8_point_inWCS[7]),
			glm::vec4(_8_point_inWCS[4]),
			glm::vec4(_8_point_inWCS[0]),
			glm::vec4(_8_point_inWCS[4]),
			glm::vec4(_8_point_inWCS[1]),
			glm::vec4(_8_point_inWCS[5]),
			glm::vec4(_8_point_inWCS[2]),
			glm::vec4(_8_point_inWCS[6]),
			glm::vec4(_8_point_inWCS[3]),
			glm::vec4(_8_point_inWCS[7])
		};

		GLuint VAO, VBO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(glm::vec4), &boundingboxvertices[0], GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), NULL);
		shader->use();
		shader->setVec3("color", glm::vec3(0.0, 1.0, 0.0));
		glDrawArrays(GL_LINES, 0, 24);
	}
	//member
	glm::vec4 min;
	glm::vec4 max;
};

class OBB {
public:
	// method
	OBB(glm::vec3 min = glm::vec3(0.0f), glm::vec3 max = glm::vec3(0.0f))
	{
		this->min = glm::vec4(min, 1.0);
		this->max = glm::vec4(max, 1.0);
		this->LCS = glm::mat4(1.0f);
	}
	~OBB()
	{
	}
	void updateOBBLCS(glm::mat4 modelMatrix)
	{
		this->LCS = modelMatrix;
	}
	AABB transfer_2_AABB() 
	{
		glm::vec3 min(FLT_MAX, FLT_MAX, FLT_MAX), max(-FLT_MAX, -FLT_MAX, -FLT_MAX);
		glm::vec4 _8_point_inLCS[8];
		this->calculate_8_point_inLCS(_8_point_inLCS);
		for (int i = 0; i < 8; i++) {
			glm::vec3 point_after_transform = this->LCS * _8_point_inLCS[i];
			for (int j = 0; j < 3; j++) {
				if (point_after_transform[j] < min[j]) {
					min[j] = point_after_transform[j];
				}
				if (point_after_transform[j] > max[j]) {
					max[j] = point_after_transform[j];
				}
			}
		}
		return AABB(min, max);
	}
	void calculate_8_point_inLCS(glm::vec4* eight_point_inWCS)
	{
		eight_point_inWCS[0] = glm::vec4(this->min.x, this->min.y, this->min.z, 1.0);
		eight_point_inWCS[1] = glm::vec4(this->min.x, this->min.y, this->max.z, 1.0);
		eight_point_inWCS[2] = glm::vec4(this->max.x, this->min.y, this->max.z, 1.0);
		eight_point_inWCS[3] = glm::vec4(this->max.x, this->min.y, this->min.z, 1.0);
		eight_point_inWCS[4] = glm::vec4(this->min.x, this->max.y, this->min.z, 1.0);
		eight_point_inWCS[5] = glm::vec4(this->min.x, this->max.y, this->max.z, 1.0);
		eight_point_inWCS[6] = glm::vec4(this->max.x, this->max.y, this->max.z, 1.0);
		eight_point_inWCS[7] = glm::vec4(this->max.x, this->max.y, this->min.z, 1.0);
	}
	void drawBoundingBox(Shader * shader)
	{
		glm::vec4 _8_point_inLCS[8];
		this->calculate_8_point_inLCS(_8_point_inLCS);
		glm::vec4 _8_point_inWCS[8];
		for (int i = 0; i < 8; i++) 
		{
			_8_point_inWCS[i] = this->LCS * _8_point_inLCS[i];
		}

		glm::vec4 boundingboxvertices[24] = {
			glm::vec4(_8_point_inWCS[0]),
			glm::vec4(_8_point_inWCS[1]),
			glm::vec4(_8_point_inWCS[1]),
			glm::vec4(_8_point_inWCS[2]),
			glm::vec4(_8_point_inWCS[2]),
			glm::vec4(_8_point_inWCS[3]),
			glm::vec4(_8_point_inWCS[3]),
			glm::vec4(_8_point_inWCS[0]),
			glm::vec4(_8_point_inWCS[4]),
			glm::vec4(_8_point_inWCS[5]),
			glm::vec4(_8_point_inWCS[5]),
			glm::vec4(_8_point_inWCS[6]),
			glm::vec4(_8_point_inWCS[6]),
			glm::vec4(_8_point_inWCS[7]),
			glm::vec4(_8_point_inWCS[7]),
			glm::vec4(_8_point_inWCS[4]),
			glm::vec4(_8_point_inWCS[0]),
			glm::vec4(_8_point_inWCS[4]),
			glm::vec4(_8_point_inWCS[1]),
			glm::vec4(_8_point_inWCS[5]),
			glm::vec4(_8_point_inWCS[2]),
			glm::vec4(_8_point_inWCS[6]),
			glm::vec4(_8_point_inWCS[3]),
			glm::vec4(_8_point_inWCS[7])
		};

		GLuint VAO, VBO;
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(glm::vec4), &boundingboxvertices[0], GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), NULL);
		shader->use();
		shader->setVec3("color", glm::vec3(0.0, 0.0, 1.0));
		glDrawArrays(GL_LINES, 0, 24);
		glBindVertexArray(0);
	}
	//member
	glm::vec4 min;
	glm::vec4 max;
	glm::mat4 LCS;
};

class Sphere {
public:
	// method
	Sphere()
	{
		this->center = glm::vec4(0.0, 0.0, 0.0, 1.0);
		this->radius = 0.0;
	}
	Sphere(glm::vec4 center, float radius)
	{
		this->center = center;
		this->radius = radius;
	}
	~Sphere()
	{
	}
	//member
	glm::vec4 center;
	float radius;
};

class Cylinder {
public:
	// method
	Cylinder() 
	{
		this->center = glm::vec4(0.0, 0.0, 0.0, 1.0);;
		this->direction = glm::vec4(0.0, 0.0, 0.0, 0.0);;
		this->radius = 0.0f;
		this->length = 0.0f;
	}
	Cylinder(glm::vec4 center, glm::vec4 direction, float radius, float length)
	{
		this->center = center;
		this->direction = direction;
		this->radius = radius;
		this->length = length;
	}
	~Cylinder()
	{

	}
	//member
	glm::vec4 center;
	glm::vec4 direction;
	float radius;
	float length;
};

class MyCollisionDetection {
public:
	MyCollisionDetection();
	~MyCollisionDetection();

	bool intersect(AABB, AABB);
	bool intersect(AABB, Sphere);
	bool intersect(AABB, Cylinder);
	bool intersect(AABB, OBB);
	bool intersect(OBB, OBB);
	bool intersect(OBB, Sphere);
	bool intersect(OBB, Cylinder);
	bool intersect(Cylinder, Sphere);
};