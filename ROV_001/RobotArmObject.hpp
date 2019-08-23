/*
 * ROV_001
 * Copyright (C) 2019 ROV Simulator
 * National Taiwan Ocean University
 * Department of Computer Science and Engineering
 * Computer Graphics Lab Chieh-Shih Chou, Shyh-Kuang Ueng
 * Copyright (C) 2019
 */
#pragma once
 // std include
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#define _USE_MATH_DEFINES // for C++  
#include <math.h>

// GL include
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Model.hpp"

// My object include
#include "MyCollisionDetection.hpp"

class RobotArmObject
{
public:
	RobotArmObject(string modelfilepath, string parameterfilepath);
	~RobotArmObject();
	void calculateBodyMatrix(glm::mat4 parentObjectModelMatrix);
	void calculateModelMatrix(glm::mat4 main_rov_modelmatrix);
	void calculateBoundingBox();
	void drawPhysicsCenter(Shader* shader);
	void draw(Shader* shader);

	//mamber
private:
	// Render Object
	Model* robotArmObjectModel;
	Model* ball;

public:
	/* Bounding Box */
	OBB obb;												/* in body space */
	AABB aabb;												/* in world space */

	/* Constant quantities */
	double mass;											/* mass M */
	double volume;											/* volume V */
	glm::vec4 mass_center;									/* The position of mass center (BCS)	*/
	glm::vec4 volume_center;								/* The position of bouyancy center (BCS)*/
	glm::mat4 Inertia;										/*  */

	// matrix
private:
	// fixed and only loaded once
	glm::vec3 offset_center_position;
	glm::mat4 offset_translate_matrix;
	bool offset_rotate_flag;
	float offest_radian;
	glm::vec3 offest_andian_rotate_axis;
	glm::mat4 offset_rotate_matrix;
	bool control_rotate_flag;
	glm::vec3 control_radian_rotate_axis;
public:
	// controlable and usually changed
	float control_radian;
	glm::mat4 control_rotate_matirx;
	glm::mat4 bodyMatrix;									/* transform robot arm into main rov */
	glm::mat4 modelMatrix;									
};

