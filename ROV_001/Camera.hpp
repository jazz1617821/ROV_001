/*
 * ROV_001
 * Copyright (C) 2019 ROV Simulator
 * National Taiwan Ocean University
 * Department of Computer Science and Engineering
 * Lab 503 Chieh-Shih Chou, Shyh-Kuang Ueng
 * Copyright (C) 2019
 */

#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#define ZOOMIN 1
#define ZOOMOUT 2

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

enum Camera_Type {
	ROV_Camera,
	Axis_Fallow,
	Fixed_Position_Fallow,
	Free
};

class Camera_t
{
public:
	// method
	Camera_t();
	~Camera_t();
	
	//view
	void setPositionPoint(float positionX, float positionY, float positionZ);
	void setFrontVector(float vectorX, float vectorY, float veYtorZ);
	void setViewUpVector(float vectorX, float vectorY, float veYtorZ);
	void setRightVector(float vectorX, float vectorY, float veYtorZ);
	glm::mat4 lookAt();

	// projection function
	// both
	void setAspect(float aspect);
	void setAspect(float width, float height);
	void setNearClipPlane(float nearClip);
	void setFarClipPlane(float farClip);
	void setClipPlane(float nearClip, float farClip);
	// perspective 
	void setFovy(float fovy);
	void zoom(int state);
	glm::mat4 perspective();
	// parallel 
	void setWidth(float width);
	glm::mat4 ortho();

	//member
	int camera_type;
	// view 
	glm::vec3 position;

	//camera coordinate 3 direction!
	glm::vec3 front;
	glm::vec3 right;
	glm::vec3 up;

	float total_yaw;
	float total_pitch;
	float delta_yaw;
	float delta_pitch;

	// both projection member
	float aspect;
	float nearClip;
	float farClip;
	// perspective projection
	float fovy;

	// parallel  projection
	float width;


	//
	void updateCameraVectors();
	void keyboardMove(Camera_Movement direction);
	void mouseMove(int xOffset, int yOffset);
private:
	
};

