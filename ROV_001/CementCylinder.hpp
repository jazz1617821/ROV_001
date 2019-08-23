/*
 * ROV_001
 * Copyright (C) 2019 ROV Simulator
 * National Taiwan Ocean University
 * Department of Computer Science and Engineering
 * Computer Graphics Lab Chieh-Shih Chou, Shyh-Kuang Ueng
 * Copyright (C) 2019
 */
#pragma once
#include "Model.hpp"
#include "Shader.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "MyCollisionDetection.hpp"
class CementCylinder
{
public:
	CementCylinder();
	~CementCylinder();

	void translate(glm::vec3);
	void rotateX(float);
	void rotateY(float);
	void rotateZ(float);
	void scaleLength(float);
	void scaleRadius(float);
	void makemodelmatrix();
	void draw(Shader*);


	// member
public:
	Cylinder cbb;
	Cylinder cbb_world;
private:
	glm::mat4 modelmatrix;
	glm::mat4 translatematrix;
	glm::mat4 rotateXmatrix;
	glm::mat4 rotateYmatrix;
	glm::mat4 rotateZmatrix;
	glm::mat4 scaleYmatrix;
	glm::mat4 scaleXZmatrix;
	Model* model;
	//Model* ball;
};

