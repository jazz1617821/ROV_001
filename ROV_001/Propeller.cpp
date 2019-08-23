/*
 * ROV_001
 * Copyright (C) 2019 ROV Simulator
 * National Taiwan Ocean University
 * Department of Computer Science and Engineering
 * Computer Graphics Lab Chieh-Shih Chou, Shyh-Kuang Ueng
 * Copyright (C) 2019
 */
#include "Propeller.hpp"



Propeller::Propeller()
{
	force_vector = glm::vec4(0.0, 0.0, 1.0, 0.0);
	force_position = glm::vec4(0.0, 0.0, 0.0, 1.0);
}

Propeller::~Propeller()
{
}

void Propeller::initialParameter(glm::vec4 force_position, glm::vec4 force_vector)
{
	this->force_vector = force_vector;
	this->force_position = force_position;
}

float Propeller::getForce(float scaler)
{
	float force = 0.0f;
	//if (scaler <= 1 && scaler >= -1)
	//{
	//	float x = scaler;
	//	force = Max_Force_KGF_CunterClockWise * x * 9.8;
	//}
	//else
	//{
	//	float x = scaler / 2.1677565271379655862476806675037;
	//	force = Max_Force_KGF_ClockWise * x * 9.8;
	//}

	float x = scaler;
	force = Max_Force_KGF_ClockWise * x * 9.8;

	return force;
}

float Propeller::getPower(float scaler)
{
	float power = 0;
	float force = this->getForce(scaler);
	bool state = ClockWise;
	if (scaler <= 1 && scaler >= -1)
	{
		state = ClockWise;
	}
	else 
	{
		state = CunterClockWise;
	}
	power = force2power(force, state);
	return power;
}

float Propeller::getRPM(float scaler)
{
	float RPM = 0;
	float force = this->getForce(scaler);
	if (scaler <= 1 && scaler >= -1)
	{
		return RPM = force_to_rpm_cunterclockwise(force);
	}
	else
	{
		return RPM = force_to_rpm_clockwise(force);
	}
}

glm::vec3 Propeller::calculateForceVector(float scaler)
{
	return this->force_vector * getForce(scaler);
}


