/*
 * ROV_001
 * Copyright (C) 2019 ROV Simulator
 * National Taiwan Ocean University
 * Department of Computer Science and Engineering
 * Computer Graphics Lab Chieh-Shih Chou, Shyh-Kuang Ueng
 * Copyright (C) 2019
 */
#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtx/vector_angle.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"
#include <iostream>
#include <cmath>

using namespace std;

#define Cd 1.05
#define Cf 1.15
#define WATER_DENSITY 1030

class PhysicEngine
{
public:
	PhysicEngine();
	~PhysicEngine();
	void initialize(double mass, glm::vec3 gravity_center, glm::mat3 Ibody, glm::mat3 Ibodyinv);
	void clear(void);
	void addGlobalForce(glm::vec3 global_force_vector,glm::vec3 force_point, glm::mat4 WorldToMotionRotateMatrix);
	void addLocalForce(glm::vec3 local_force_vector, glm::vec3 force_point, glm::mat4 MotionToWorldRotateMatrix);
	glm::vec3 calculateDragForce(glm::vec3 model_size, glm::mat4 WorldToMotionRotateMatrix);
	void calculateResistanceForce(glm::vec3 model_size, glm::vec3 mass_center, glm::mat4 MotionToWorldRotateMatrix);
	void calculateLinearMotion(float deltaT);
	void calculateAngularMotion(float deltaT);
	void Run(float deltaT);
	void undoMotion(void);
	//void unMovePositiveY(void);
	//void unMoveNegativeY(void);
	//void unMove(float);
	//void unRotate(void);


public:
	/* Linear Motion is calculate in World Space.	*/
	/* Angular Motion is calculate in Motion Space. */

	/* Constant quantities */
	double mass;											/* mass M */
	glm::vec3 mass_center;									/* Always at (0,0,0) in Motion space.	*/
	glm::mat3 Inertia;										/* Tensor of Inertia */
	glm::mat3 InertiaInv;									/* Inverse of Inertia Tensor */

	/* State variables */
	glm::vec3 x;											/* Displacement of model center										*/
	glm::vec3 r;											/* The rotation angle amount (x,y,z)								*/
	//glm::vec3 P;											/* P(t) Linear momentum P = mv										*/
	//glm::vec3 L;											/* L(t) Angular momentum											*/

	/* Computed quantities 	*/
	glm::vec3 force;											/* F(t) total force.	Use to calculate Linear Motion.				*/
	glm::vec3 torque;										/* τ(t)	total torque.	Use to calculate Angular Motion.			*/

	/* Derived quantities (auxiliary variables)	*/
	glm::vec3 a_last;
	glm::vec3 a;												/* a(t) acceleration											*/
	glm::vec3 alpha_last;
	glm::vec3 alpha;											/* α(t) angular acceleration */
	glm::vec3 v_last;
	glm::vec3 v;												/* v(t) velocity												*/
	glm::vec3 omega_last;
	glm::vec3 omega;											/* w(t) angular velocity										*/

};

