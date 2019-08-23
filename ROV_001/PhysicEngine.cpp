/*
 * ROV_001
 * Copyright (C) 2019 ROV Simulator
 * National Taiwan Ocean University
 * Department of Computer Science and Engineering
 * Computer Graphics Lab Chieh-Shih Chou, Shyh-Kuang Ueng
 * Copyright (C) 2019
 */
#include "PhysicEngine.hpp"


PhysicEngine::PhysicEngine()
{
	/* Constant quantities */
	mass = 0.0;
	mass_center = glm::vec3(0.0f);
	Inertia = glm::mat3(1.0f);
	InertiaInv = glm::mat3(1.0f);

	/* State variables */
	x = glm::vec3(0.0, 0.0, 0.0);
	r = glm::vec3(0.0f);
	//P = glm::vec3(0.0f);
	//L = glm::vec3(0.0f);

	/* Computed quantities */
	force = glm::vec3(0.0f);
	torque = glm::vec3(0.0f);

	/* Derived quantities (auxiliary variables) */
	a = a_last = glm::vec3(0.0f);
	alpha = alpha_last = glm::vec3(0.0f);
	v = v_last = glm::vec3(0.0f);
	omega = omega_last = glm::vec3(0.0f);
}

PhysicEngine::~PhysicEngine()
{
}

void PhysicEngine::initialize(double mass, glm::vec3 mass_center, glm::mat3 Inertia, glm::mat3 InertiaInv)
{
	this->mass = mass;
	this->mass_center = mass_center;
	this->Inertia = Inertia;
	this->InertiaInv = InertiaInv;
}

void PhysicEngine::clear(void)
{
	mass = 0.0;
	mass_center = glm::vec3(0.0f);
	Inertia = glm::mat3(1.0f);
	InertiaInv = glm::mat3(1.0f);

	/* State variables */
	x = glm::vec3(0.0, 0.0, 0.0);
	r = glm::vec3(0.0f);
	//P = glm::vec3(0.0f);
	//L = glm::vec3(0.0f);

	/* Computed quantities */
	force = glm::vec3(0.0f);
	torque = glm::vec3(0.0f);

	/* Derived quantities (auxiliary variables) */
	a = glm::vec3(0.0f);
	alpha = glm::vec3(0.0f);
	v = glm::vec3(0.0f);
	omega = glm::vec3(0.0f);
}

void PhysicEngine::addGlobalForce(glm::vec3 global_force_vector, glm::vec3 force_point, glm::mat4 WorldToMotionRotateMatrix)
{
	// in world space
	this->force += global_force_vector;

	// in motion space
	glm::vec3 r = (force_point - this->mass_center);
	this->torque += glm::cross(r, glm::mat3(WorldToMotionRotateMatrix) * global_force_vector);
}

void PhysicEngine::addLocalForce(glm::vec3 local_force_vector, glm::vec3 force_point, glm::mat4 MotionToWorldRotateMatrix)
{
	// in world space
	this->force += glm::mat3(MotionToWorldRotateMatrix) * local_force_vector;

	// in motion space
	glm::vec3 r = (force_point - this->mass_center);
	this->torque += glm::cross(r, local_force_vector);
}

glm::vec3 PhysicEngine::calculateDragForce(glm::vec3 model_size, glm::mat4 WorldToMotionRotateMatrix)
{
	glm::vec3 reference_area = glm::vec3(model_size.y * model_size.z * 0.5, model_size.x * model_size.z * 0.7, model_size.x * model_size.y * 0.5);
	float A = fabs(glm::dot(reference_area, (glm::length(v) ? glm::normalize(v) : glm::vec3(0.0))));
	glm::vec3 drag_force_direction = -glm::normalize(v);
	float drag_force_magnitude = glm::length(v) * glm::length(v);
	glm::vec3 world_drag = (glm::length(v) == 0 ? glm::vec3(0.0f) : (float)(0.5 * WATER_DENSITY * Cd * A) * drag_force_magnitude * drag_force_direction);
	/*cout << "v magnitude: " << glm::length(v) << endl;
	cout << "drag magnitude: " << drag_force_magnitude << endl;
	cout << "v direction: " << "<" << glm::normalize(v).x << ", " << glm::normalize(v).y << ", " << glm::normalize(v).z << ">" << endl;
	cout << "drag direction: " << "<" << drag_force_direction.x << ", " << drag_force_direction.y << ", " << drag_force_direction.z << ">" << endl;
	cout << "World Drag: " << world_drag.x << ", " << world_drag.y << ", " << world_drag.z << endl;*/

	this->force += world_drag;

	return world_drag;
}

void PhysicEngine::calculateResistanceForce(glm::vec3 model_size , glm::vec3 mass_center, glm::mat4 MotionToWorldRotateMatrix)
{
	glm::vec3 r[6] = {
		glm::vec3(0.5f * model_size.x - mass_center.x, 0.0f, 0.0f),		// rx
		glm::vec3(0.5f * -model_size.x - mass_center.x, 0.0f, 0.0f),	// rxi
		glm::vec3(0.0f, 0.5f * model_size.y - mass_center.y, 0.0f),		// ry
		glm::vec3(0.0f, 0.5f * -model_size.y - mass_center.y, 0.0f),	// ryi
		glm::vec3(0.0f, 0.0f, 0.5f * model_size.z - mass_center.z),		// rz
		glm::vec3(0.0f, 0.0f, 0.5f * -model_size.z - mass_center.z)		// rzi
	};

	float A[6] = { 
		fabs(model_size.y * model_size.z * (model_size.x - mass_center.x / model_size.x)),		// yz(+x) surface
		fabs(model_size.y * model_size.z * (-model_size.x - mass_center.x / model_size.x)),		// yz(-x) surface
		fabs(model_size.x * model_size.z * (model_size.y - mass_center.y / model_size.y)),		// xz(+y) surface
		fabs(model_size.x * model_size.z * (-model_size.y - mass_center.y / model_size.y)),		// xz(-y) surface
		fabs(model_size.x * model_size.y * (model_size.z - mass_center.z / model_size.z)),		// xy(+z) surface
		fabs(model_size.x * model_size.y * (-model_size.z - mass_center.z / model_size.z))		// xy(-z) surface
	};


	for (int i = 0; i < 6; i++) {
		glm::vec3 v = glm::cross(this->omega, r[i]);
		float A = 1.0;								// need to compute
		glm::vec3 resistance_force = (float)(0.5 * WATER_DENSITY * Cf * A) * (glm::length(v) == 0 ? glm::vec3(0.0f) : (glm::length(v) * glm::length(v) * -glm::normalize(v)));
		addLocalForce(resistance_force, r[i], MotionToWorldRotateMatrix);
	}

}

void PhysicEngine::calculateLinearMotion(float dt)
{
	if ((isnormal(force.x) || force.x < 0.000000001) && (isnormal(force.y) || force.y < 0.000000001) && (isnormal(force.z) || force.z < 0.000000001)) {
		// m / s
		this->v_last = this->v;
		this->a_last = this->a;
		glm::vec3 v0 = this->v;
		this->a = this->force * (float)(1 / this->mass);
		this->v = v0 + this->a * dt;
		this->x = this->v * dt;
	}
}

void PhysicEngine::calculateAngularMotion(float dt)
{
	if ((isnormal(torque.x) || torque.x < 0.000000001) && (isnormal(torque.y)|| torque.y < 0.000000001) && (isnormal(torque.z) || torque.z < 0.000000001)) {
		// rad / s
		this->omega_last = this->omega;
		this->alpha_last = this->alpha;
		glm::vec3 omega0 = this->omega;
		this->alpha = this->InertiaInv * (this->torque - glm::cross(omega0, this->Inertia * omega0));		// a = I-1 * (N - W x (I * W))
		this->omega = omega0 + this->alpha * dt;
		glm::vec3 temp = this->omega * dt;
		this->r = this->omega * dt;
	}
}

void PhysicEngine::Run(float deltaT)
{
	calculateLinearMotion(deltaT);
	calculateAngularMotion(deltaT);

	this->force = glm::vec3(0.0f);
	this->torque = glm::vec3(0.0f);
}

void PhysicEngine::undoMotion(void)
{
	this->v = this->v_last;
	this->a = this->a_last;

	this->omega = this->omega_last;
	this->alpha = this->alpha_last;
}

//void PhysicEngine::unMovePositiveY(void)
//{
//	this->x += glm::vec3(-this->frame_displacement.x, fabs(this->frame_displacement.y), -this->frame_displacement.z);
//}
//
//void PhysicEngine::unMoveNegativeY(void)
//{
//	this->x += glm::vec3(-this->frame_displacement.x, -fabs(this->frame_displacement.y), -this->frame_displacement.z);
//}
//
//void PhysicEngine::unMove(float step)
//{
//	this->x += step * glm::vec3(-this->frame_displacement.x, -this->frame_displacement.y, -this->frame_displacement.z);
//}
//
//void PhysicEngine::unRotate(void)
//{
//	this->r = glm::vec3(0.0f);
//}
