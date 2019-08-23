/*
 * ROV_001
 * Copyright (C) 2019 ROV Simulator
 * National Taiwan Ocean University
 * Department of Computer Science and Engineering
 * Computer Graphics Lab Chieh-Shih Chou, Shyh-Kuang Ueng
 * Copyright (C) 2019
 */
#include "Camera.hpp"

#include <iostream>

using namespace std;

#define X 0
#define Y 1
#define Z 2



Camera_t::Camera_t()
{
	this->camera_type = ROV_Camera;
	this->setPositionPoint(0, 0, 0);
	this->setFrontVector(0, 0, 1);
	this->setViewUpVector(0, 1, 0);

	this->setWidth(5);
	this->setFovy(45);
	this->setAspect(1);
	this->setClipPlane(0.1,10000);

	this->total_yaw = 0.0f;
	this->total_pitch = 0.0f;
	this->delta_yaw = 0.0f;
	this->delta_pitch = 0.0f;

	//set front, right ,up vector
	//updateCameraVectors();
}


Camera_t::~Camera_t()
{
}

void Camera_t::setPositionPoint(float x, float y, float z)
{
	this->position[X] = x;
	this->position[Y] = y;
	this->position[Z] = z;
}

void Camera_t::setFrontVector(float x, float y, float z)
{
	this->front.x = x;
	this->front.y = y;
	this->front.z = z;
}

void Camera_t::setViewUpVector(float x, float y, float z)
{
	this->up.x = x;
	this->up.y = y;
	this->up.z = z;
}

void Camera_t::setRightVector(float x, float y, float z)
{
	this->right.x = x;
	this->right.y = y;
	this->right.z = z;
}

glm::mat4 Camera_t::lookAt()
{
	glm::mat4 view = glm::lookAt(this->position, this->position + front, this->up);
	return view;
}

void Camera_t::setFovy(float fovy)
{
	this->fovy = fovy;
}

void Camera_t::zoom(int state)
{
	if (state == ZOOMIN) {
		if (this->fovy >= 30)
			this->fovy -= 0.1;
	}
	if (state == ZOOMOUT) {
		if (this->fovy <= 90)
			this->fovy += 0.1;
	}
}

void Camera_t::setAspect(float aspect)
{
	this->aspect = aspect;
}

void Camera_t::setAspect(float width, float height)
{
	this->aspect = width / height;
}

void Camera_t::setNearClipPlane(float nearClip)
{
	this->nearClip = nearClip;
}

void Camera_t::setFarClipPlane(float farClip)
{
	this->farClip = farClip;
}

void Camera_t::setClipPlane(float nearClip, float farClip)
{
	this->nearClip = nearClip;
	this->farClip = farClip;
}

glm::mat4 Camera_t::perspective()
{
	return glm::perspective(
		glm::radians(this->fovy),
		this->aspect,
		this->nearClip,
		this->farClip
	);

}
void Camera_t::setWidth(float width)
{
	if (this->width >= 2) {
		this->width = width;
	}
	else {
		this->width = 2;
	}
}
glm::mat4 Camera_t::ortho()
{
	return glm::ortho(-this->width / 2, this->width / 2, -(this->width / this->aspect) / 2, (this->width / this->aspect) / 2, this->nearClip, this->farClip);
}
void Camera_t::updateCameraVectors()
{
	glm::mat3 rotate_yaw = (glm::mat3)glm::rotate(glm::mat4(1.0f), glm::radians(-this->delta_yaw), this->up);
	glm::mat3 rotate_pitch = (glm::mat3)glm::rotate(glm::mat4(1.0f), glm::radians(this->delta_pitch), this->right);
	
	this->front = glm::normalize(rotate_pitch * rotate_yaw * front);
	this->right = glm::normalize(glm::cross(this->front, glm::vec3(0.0, 1.0, 0.0)));
	this->up = glm::normalize(glm::cross(this->right, this->front));

	this->delta_yaw = this->delta_pitch = 0;
}

void Camera_t::keyboardMove(Camera_Movement direction)
{
	if (direction == FORWARD)
		this->position += this->front * 0.05f;
	if (direction == BACKWARD)
		this->position -= this->front * 0.05f;
	if (direction == LEFT)
		this->position -= this->right * 0.05f;
	if (direction == RIGHT)
		this->position += this->right * 0.05f;
}
void Camera_t::mouseMove(int xOffset, int yOffset)
{
	this->delta_yaw = xOffset * 0.05;
	this->delta_pitch = yOffset * 0.05;
	if (this->total_pitch > 89.0f)
	{
		this->delta_pitch = 0.0f;
		this->total_pitch = 89.0f;
	}
	if (this->total_pitch < -89.0f)
	{
		this->delta_pitch = 0.0;
		this->total_pitch = -89.0f;
	}
	this->total_yaw += this->delta_yaw;
	this->total_pitch += this->delta_pitch;


	//cout << "yaw:" << this->yaw << endl;
	//cout << "pitch:" << this->pitch << endl;
}
