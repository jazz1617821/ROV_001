/*
 * ROV_001
 * Copyright (C) 2019 ROV Simulator
 * National Taiwan Ocean University
 * Department of Computer Science and Engineering
 * Computer Graphics Lab Chieh-Shih Chou, Shyh-Kuang Ueng
 * Copyright (C) 2019
 */
#include "Light.hpp"

int Light::light_amount = 0;
int Light::point_light_amount = 0;
int Light::direct_light_amount = 0;
int Light::spot_light_amount = 0;

Light::Light() 
{
}

Light::Light(int light_class)
{
	light_amount++;
	if (light_class == POINTLIGHT) {
		this->light_class = POINTLIGHT;
		this->ID = point_light_amount;
		point_light_amount++;
	}else if (light_class == DIRECTIONLIGHT) {
		this->light_class = DIRECTIONLIGHT;
		this->ID = direct_light_amount;
		direct_light_amount++;
	}else if (light_class == SPOTLIGHT) {
		this->light_class = SPOTLIGHT;
		this->ID = spot_light_amount;
		spot_light_amount++;
	}

	position = glm::vec4(0.0, 0.0, 0.0, 1.0);
	direction = glm::vec4(1.0, 0.0, 0.0, 0.0);

	cutoff_angle = 45.0;
	exponent = 5.0;

	ambient = glm::vec4(0.1, 0.1, 0.1, 1.0);
	diffuse = glm::vec4(0.7, 0.7, 0.7, 1.0);
	specular = glm::vec4(0.2, 0.2, 0.2, 1.0);

	// distance = 100
	constant = 1.0;
	linear = 0.022;
	quadratic = 0.0019;
}


Light::~Light()
{
	if (this->light_class == POINTLIGHT) {
		point_light_amount--;
	}
	if (this->light_class == DIRECTIONLIGHT) {
		direct_light_amount--;
	}
	if (this->light_class == SPOTLIGHT) {
		spot_light_amount--;
	}
	light_amount--;
}

void Light::setPosition(glm::vec4 position)
{
	this->position = position;
}

void Light::setDirection(glm::vec4 direction)
{
	this->direction = direction;
}

void Light::setCutoffAngle(float cutoff_angle)
{
	this->cutoff_angle = cutoff_angle;
}

void Light::setExponent(float exponent)
{
	this->exponent = exponent;
}

void Light::setAmbient(glm::vec4 ambient)
{
	this->ambient = ambient;
}

void Light::setDiffuse(glm::vec4 diffuse)
{
	this->diffuse = diffuse;
}

void Light::setSpecular(glm::vec4 specular)
{
	this->specular = specular;
}

void Light::setAttenuation_Constant(float value)
{
	constant = value;
}

void Light::setAttenuation_Linear(float value)
{
	linear = value;
}

void Light::setAttenuation_Quadratic(float value)
{
	quadratic = value;
}

void Light::populatePosition(Shader* shader)
{
	 std::string parameter_container_name;
	if (light_class == POINTLIGHT) {
		parameter_container_name = "pointLights";
	}
	if (light_class == DIRECTIONLIGHT) {
		return;
	}
	if (light_class == SPOTLIGHT) {
		parameter_container_name = "spotLights";
	}

	parameter_container_name += "[" + std::to_string(this->ID) + "].position";
	shader->setVec3(parameter_container_name.c_str(), this->position);
}

void Light::populateDirection(Shader * shader)
{
	 std::string parameter_container_name;
	if (light_class == POINTLIGHT) {
		return;
	}
	if (light_class == DIRECTIONLIGHT) {
		parameter_container_name = "directLights";
	}
	if (light_class == SPOTLIGHT) {
		parameter_container_name = "spotLights";
	}

	parameter_container_name += "[" + std::to_string(this->ID) + "].direction";
	shader->setVec3(parameter_container_name.c_str(), this->direction);
}

void Light::populateCutoffAngle(Shader * shader)
{
	 std::string parameter_container_name;
	if (light_class == SPOTLIGHT) {
		parameter_container_name = "spotLights";
	}
	else {
		return;
	}

	parameter_container_name += "[" + std::to_string(this->ID) + "].cutoff_angle";
	shader->setFloat(parameter_container_name.c_str(), this->cutoff_angle);
}

void Light::populateExponent(Shader * shader)
{
	 std::string parameter_container_name;
	if (light_class == SPOTLIGHT) {
		parameter_container_name = "spotLights";
	}
	else {
		return;
	}

	parameter_container_name += "[" + std::to_string(this->ID) + "].exponent";
	shader->setFloat(parameter_container_name.c_str(), this->cutoff_angle);
}

void Light::populateAmbient(Shader * shader)
{
	 std::string parameter_container_name;
	if (light_class == POINTLIGHT) {
		parameter_container_name = "pointLights";
	}
	if (light_class == DIRECTIONLIGHT) {
		parameter_container_name = "directLights";
	}
	if (light_class == SPOTLIGHT) {
		parameter_container_name = "spotLights";
	}

	parameter_container_name += "[" + std::to_string(this->ID) + "].ambient";
	shader->setVec3(parameter_container_name.c_str(), this->ambient);
}

void Light::populateDiffuse(Shader * shader)
{
	 std::string parameter_container_name;
	if (light_class == POINTLIGHT) {
		parameter_container_name = "pointLights";
	}
	if (light_class == DIRECTIONLIGHT) {
		parameter_container_name = "directLights";
	}
	if (light_class == SPOTLIGHT) {
		parameter_container_name = "spotLights";
	}

	parameter_container_name += "[" + std::to_string(this->ID) + "].diffuse";
	shader->setVec3(parameter_container_name.c_str(), this->diffuse);
}

void Light::populateSpecular(Shader * shader)
{
	 std::string parameter_container_name;
	if (light_class == POINTLIGHT) {
		parameter_container_name = "pointLights";
	}
	if (light_class == DIRECTIONLIGHT) {
		parameter_container_name = "directLights";
	}
	if (light_class == SPOTLIGHT) {
		parameter_container_name = "spotLights";
	}

	parameter_container_name += "[" + std::to_string(this->ID) + "].specular";
	shader->setVec3(parameter_container_name.c_str(), this->specular);
}

void Light::populateAttenuation_Constant(Shader * shader)
{
	 std::string parameter_container_name;
	if (light_class == POINTLIGHT) {
		parameter_container_name = "pointLights";
	}
	if (light_class == DIRECTIONLIGHT) {
		return;
	}
	if (light_class == SPOTLIGHT) {
		parameter_container_name = "spotLights";
	}

	parameter_container_name += "[" + std::to_string(this->ID) + "].constant";
	shader->setFloat(parameter_container_name.c_str(), this->constant);
}

void Light::populateAttenuation_Linear(Shader * shader)
{
	 std::string parameter_container_name;
	if (light_class == POINTLIGHT) {
		parameter_container_name = "pointLights";
	}
	if (light_class == DIRECTIONLIGHT) {
		return;
	}
	if (light_class == SPOTLIGHT) {
		parameter_container_name = "spotLights";
	}

	parameter_container_name += "[" + std::to_string(this->ID) + "].linear";
	shader->setFloat(parameter_container_name.c_str(), this->linear);
}

void Light::populateAttenuation_Quadratic(Shader *shader)
{
	 std::string parameter_container_name;
	if (light_class == POINTLIGHT) {
		parameter_container_name = "pointLights";
	}
	if (light_class == DIRECTIONLIGHT) {
		return;
	}
	if (light_class == SPOTLIGHT) {
		parameter_container_name = "spotLights";
	}

	parameter_container_name += "[" + std::to_string(this->ID) + "].quadratic";
	shader->setFloat(parameter_container_name.c_str(), this->quadratic);
}

void Light::populateAllParameter(Shader * shader)
{
	 std::string parameter_container_name, light_class_name;
	if (light_class == POINTLIGHT) {
		light_class_name = "pointLights";
		parameter_container_name = light_class_name + "[" + std::to_string(this->ID) + "].position";
		shader->setVec3(parameter_container_name.c_str(), glm::vec3(this->position));
	}
	if (light_class == DIRECTIONLIGHT) {
		light_class_name = "directLights";
		parameter_container_name = light_class_name + "[" + std::to_string(this->ID) + "].direction";
		shader->setVec3(parameter_container_name.c_str(), glm::vec3(this->direction));
	}
	if (light_class == SPOTLIGHT) {
		light_class_name = "spotLights";
		parameter_container_name = light_class_name + "[" + std::to_string(this->ID) + "].position";
		shader->setVec3(parameter_container_name.c_str(), glm::vec3(this->position));
		parameter_container_name = light_class_name + "[" + std::to_string(this->ID) + "].direction";
		shader->setVec3(parameter_container_name.c_str(), glm::vec3(this->direction));
		parameter_container_name = light_class_name + "[" + std::to_string(this->ID) + "].cutoff_angle";
		shader->setFloat(parameter_container_name.c_str(), this->cutoff_angle);
		parameter_container_name = light_class_name + "[" + std::to_string(this->ID) + "].exponent";
		shader->setFloat(parameter_container_name.c_str(), this->exponent);
	}
	parameter_container_name = light_class_name + "[" + std::to_string(this->ID) + "].ambient";
	shader->setVec3(parameter_container_name.c_str(), glm::vec3(this->ambient));
	parameter_container_name = light_class_name + "[" + std::to_string(this->ID) + "].diffuse";
	shader->setVec3(parameter_container_name.c_str(), glm::vec3(this->diffuse));
	parameter_container_name = light_class_name + "[" + std::to_string(this->ID) + "].specular";
	shader->setVec3(parameter_container_name.c_str(), glm::vec3(this->specular));

	
	parameter_container_name = light_class_name + "[" + std::to_string(this->ID) + "].constant";
	shader->setFloat(parameter_container_name.c_str(), this->constant);
	parameter_container_name = light_class_name + "[" + std::to_string(this->ID) + "].linear";
	shader->setFloat(parameter_container_name.c_str(), this->linear);
	parameter_container_name = light_class_name + "[" + std::to_string(this->ID) + "].quadratic";
	shader->setFloat(parameter_container_name.c_str(), this->quadratic);
	
}


