/*
 * ROV_001
 * Copyright (C) 2019 ROV Simulator
 * National Taiwan Ocean University
 * Department of Computer Science and Engineering
 * Computer Graphics Lab Chieh-Shih Chou, Shyh-Kuang Ueng
 * Copyright (C) 2019
 */
#include "CementCylinder.hpp"


CementCylinder::CementCylinder()
{
	cbb.center = glm::vec4(0.0f);
	cbb.center.w = 1.0;
	cbb.direction = glm::vec4(0.0, 1.0, 0.0, 0.0);
	cbb.length = 80.0;
	cbb.radius = 2.5;
	cbb_world = cbb;
	modelmatrix = glm::mat4(1.0f);
	translatematrix = glm::mat4(1.0f);
	rotateXmatrix = glm::mat4(1.0f);
	rotateYmatrix = glm::mat4(1.0f);
	rotateZmatrix = glm::mat4(1.0f);
	scaleYmatrix = glm::mat4(1.0f);
	scaleXZmatrix = glm::mat4(1.0f);
	model = new Model("./Model/Scene/CementCylinder/Cement_Cylinder.obj");
	//ball = new Model("./Model/Scene/ball/ball.obj");
}

CementCylinder::~CementCylinder()
{
	delete this->model;
}

void CementCylinder::translate(glm::vec3 offset)
{
	this->translatematrix = glm::translate(glm::mat4(1.0), offset);
	makemodelmatrix();
}

void CementCylinder::rotateX(float angle)
{
	this->rotateXmatrix = glm::rotate(glm::mat4(1.0), glm::radians(angle), glm::vec3(1.0, 0.0, 0.0));
	makemodelmatrix();
}

void CementCylinder::rotateY(float angle)
{
	this->rotateXmatrix = glm::rotate(glm::mat4(1.0), glm::radians(angle), glm::vec3(0.0, 1.0, 0.0));
	makemodelmatrix();
}

void CementCylinder::rotateZ(float angle)
{
	this->rotateXmatrix = glm::rotate(glm::mat4(1.0), glm::radians(angle), glm::vec3(0.0, 0.0, 1.0));
	makemodelmatrix();
}

void CementCylinder::scaleLength(float size)
{
	this->scaleYmatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0, size, 1.0));
	cbb_world.length = size * cbb.length;
	makemodelmatrix();
}

void CementCylinder::scaleRadius(float size)
{
	this->scaleXZmatrix = glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0, size));
	cbb_world.radius = size * cbb.radius;
	makemodelmatrix();
}

void CementCylinder::makemodelmatrix()
{
	this->modelmatrix = rotateZmatrix * rotateYmatrix * rotateXmatrix * scaleXZmatrix * scaleYmatrix * translatematrix;
	
	cbb_world.center = this->modelmatrix * cbb.center;
	cbb_world.direction = glm::normalize(this->modelmatrix * cbb.direction);
}

void CementCylinder::draw(Shader * shader)
{
	shader->use();
	shader->setMat4("model", this->modelmatrix);
	this->model->Draw(*shader);

}
