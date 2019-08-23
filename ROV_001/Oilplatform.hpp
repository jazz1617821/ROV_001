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
#include "CementCylinder.hpp"
class Oilplatform
{
public:
	Oilplatform();
	~Oilplatform();
	
	void translate(glm::vec3);
	void draw(Shader*);

	bool intersect(AABB);

private:
	AABB aabb;
	CementCylinder* MainCylinder[4];

	// my bounding box object
	MyCollisionDetection mycdObject;
};

