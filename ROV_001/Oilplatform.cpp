/*
 * ROV_001
 * Copyright (C) 2019 ROV Simulator
 * National Taiwan Ocean University
 * Department of Computer Science and Engineering
 * Computer Graphics Lab Chieh-Shih Chou, Shyh-Kuang Ueng
 * Copyright (C) 2019
 */
#include "Oilplatform.hpp"

glm::vec3 MainCylinderPositionOffset[4] = {
	glm::vec3(-5.0, -40.0, 5.0),
	glm::vec3(5.0, -40.0, 5.0),
	glm::vec3(5.0, -40.0, -5.0),
	glm::vec3(-5.0, -40.0, -5.0)
};



Oilplatform::Oilplatform()
{
	// main Cylinder
	MainCylinder[0] = new CementCylinder();
	MainCylinder[0]->translate(MainCylinderPositionOffset[0]);
	MainCylinder[1] = new CementCylinder();
	MainCylinder[1]->translate(MainCylinderPositionOffset[1]);
	MainCylinder[2] = new CementCylinder();
	MainCylinder[2]->translate(MainCylinderPositionOffset[2]);
	MainCylinder[3] = new CementCylinder();
	MainCylinder[3]->translate(MainCylinderPositionOffset[3]);

}


Oilplatform::~Oilplatform()
{
}

void Oilplatform::translate(glm::vec3)
{
	
}

void Oilplatform::draw(Shader * shader)
{
	// draw main cylinder
	for (int i = 0; i < 4; i++) {
		MainCylinder[i]->draw(shader);
	}
}

bool Oilplatform::intersect(AABB rov_aabb_world)
{

	for (int i = 0; i < 4; i++) {
		if (mycdObject.intersect(rov_aabb_world, MainCylinder[i]->cbb_world)) 
		{
			return true;
		}
	}
	return false;
}
