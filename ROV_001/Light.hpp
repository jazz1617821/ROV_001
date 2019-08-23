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
#include "glm/gtc/type_ptr.hpp"
#include "Shader.hpp"
#include <iostream>

#define POINTLIGHT 0
#define DIRECTIONLIGHT 1
#define SPOTLIGHT 2

class Light
{
public:
	// 
	static int light_amount;
	static int point_light_amount;
	static int direct_light_amount;
	static int spot_light_amount;

	Light();
	Light(int light_class);
	~Light();

	// set variable
	void setPosition(glm::vec4);
	void setDirection(glm::vec4);
	void setCutoffAngle(float);
	void setExponent(float);
	void setAmbient(glm::vec4);
	void setDiffuse(glm::vec4);
	void setSpecular(glm::vec4);
	void setAttenuation_Constant(float);
	void setAttenuation_Linear(float);
	void setAttenuation_Quadratic(float);

	// populate parameter into 
	void populatePosition(Shader*);
	void populateDirection(Shader*);
	void populateCutoffAngle(Shader*);
	void populateExponent(Shader*);
	void populateAmbient(Shader*);
	void populateDiffuse(Shader*);
	void populateSpecular(Shader*);
	void populateAttenuation_Constant(Shader*);
	void populateAttenuation_Linear(Shader*);
	void populateAttenuation_Quadratic(Shader*);
	void populateAllParameter(Shader*);
	

	// member
	GLuint ID;						// 
	int light_class;				

private:

	// Basic geometry parameter
	glm::vec4 position;					// point light, spot light
	glm::vec4 direction;					// direction light, spot light

	// Spot light parameter
	float cutoff_angle;
	float exponent;

	// color parameter
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;

	float constant;
	float linear;
	float quadratic;
};

