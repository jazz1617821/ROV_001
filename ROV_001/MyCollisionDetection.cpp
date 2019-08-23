/*
 * ROV_001
 * Copyright (C) 2019 ROV Simulator
 * National Taiwan Ocean University
 * Department of Computer Science and Engineering
 * Computer Graphics Lab Chieh-Shih Chou, Shyh-Kuang Ueng
 * Copyright (C) 2019
 */
#include "MyCollisionDetection.hpp"

MyCollisionDetection::MyCollisionDetection() 
{
}

MyCollisionDetection::~MyCollisionDetection()
{
}

bool MyCollisionDetection::intersect(AABB aabb_1, AABB aabb_2)
{
	return false;
}

bool MyCollisionDetection::intersect(AABB aabb, Sphere sphere)
{
	return false;
}

bool MyCollisionDetection::intersect(AABB aabb, Cylinder cylinder)
{
	AABB temp_aabb;
	// ckeck cylinder is up to Y or not?
	if (glm::l2Norm(glm::vec3(cylinder.direction) - glm::vec3(0.0, 1.0, 0.0)) >= 0.001) {
		// project aabb to cylinder (use Cylinder's direction as axis-Y)
		glm::mat4 projectionMatrix;
		glm::vec3 rotateAxis = glm::cross(glm::vec3(cylinder.direction), glm::vec3(0.0, 1.0, 0.0));
		projectionMatrix = glm::rotate(glm::mat4(1.0), glm::radians(acos(glm::dot(glm::normalize(glm::vec3(cylinder.direction)), glm::vec3(0.0, 1.0, 0.0)))), rotateAxis);
		temp_aabb = aabb.projectionTransfer(projectionMatrix);
	}
	else {
		temp_aabb = aabb;
	}
	// check projected aabb is in the cylinder or not?
	if (!(temp_aabb.max.y < cylinder.center.y - (cylinder.length / 2)|| temp_aabb.min.y > cylinder.center.y + (cylinder.length / 2))) {
		// check 8 point's distance to line 
		glm::vec4 eight_point[8];
		temp_aabb.calculate_8_point_inWCS(eight_point);
		for (int i = 0; i < 8; i++) 
		{
			glm::vec3 PT = glm::vec3(cylinder.center) + glm::vec3(0.0, cylinder.length / 2, 0.0) - glm::vec3(eight_point[i]);
			glm::vec3 PB = glm::vec3(cylinder.center) - glm::vec3(0.0, cylinder.length / 2, 0.0) - glm::vec3(eight_point[i]);
			glm::vec3 BT = PT - PB;
			float distance = (glm::l2Norm(glm::cross(PT, PB)) / glm::l2Norm(BT));
			if (distance <= cylinder.radius) {
				return true;
			}
		}
	}

	return false;
}

bool MyCollisionDetection::intersect(AABB, OBB)
{
	return false;
}

bool MyCollisionDetection::intersect(OBB, OBB)
{
	return false;
}

bool MyCollisionDetection::intersect(OBB, Sphere)
{
	return false;
}

bool MyCollisionDetection::intersect(OBB, Cylinder)
{
	return false;
}

bool MyCollisionDetection::intersect(Cylinder cylinder, Sphere sphere)
{
	return false;
}
