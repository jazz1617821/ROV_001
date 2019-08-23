/*
 * ROV_001
 * Copyright (C) 2019 ROV Simulator
 * National Taiwan Ocean University
 * Department of Computer Science and Engineering
 * Computer Graphics Lab Chieh-Shih Chou, Shyh-Kuang Ueng
 * Copyright (C) 2019
 */
#pragma once
// std include
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#define _USE_MATH_DEFINES // for C++  
#include <math.h>
#include <float.h>

// GL include
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"
#include "Model.hpp"
#include "Light.hpp"

// My object include
#include "PhysicEngine.hpp"
//#include "VoxelData.hpp"
#include "MyCollisionDetection.hpp"
#include "Propeller.hpp"
#include "PropellerVector.hpp"
#include "MyPID.hpp"
#include "RobotArmObject.hpp"
extern "C"
{
#include "solverdefinition.h"
#include "vec_mtx.h"
}


#define X 0
#define Y 1
#define Z 2

enum MtionName { Sway, Heave, Surge, Pitch, Yaw, Roll, NumMotion };
enum RobotArmName { RobotArmFrameStructure, ControlJoint1, Arm1, Arm2, ControlJoint2, Paw1, Paw2, NumRobotArmName};

using namespace std;
class ROV
{
	// method
public:
	ROV();
	~ROV();
	/*initial*/
	bool loadParameter();
	void initialPID();
	void initialRobotArm();

	/*draw*/
	void drawMainROV(Shader* shader);
	void drawRobotArm(Shader* shader);
	void drawBoundingBox(Shader* shader, int mode);
	void drawPhysicsCenter(Shader* shader);

	/*calculate*/
	void clearMotionPower();
	void runPID(glm::vec3 global_target_position, glm::vec3 target_orientation, glm::vec3 global_target_velocity, glm::vec3 target_angularVelocity, float time_interval);
	void addHydroStaticForce();
	glm::vec3 addDragForce();
	void addPropellerForce();
	void calculateTransferMatrix();
	void calculateRobotArmTransferMatrix();
	void calculateRotateAngle();
	void calculateDisplacement();
	void calculateBoundingBox();
	void calculateSumPhysicsQuantities();
	void calculateAveragePhysicsCenter();
	void calculateAverageInertia();
	void calculatePropellerVector();
	void resetLocation();
	void undoModelMatrix();
	
	/**/
	void controlRobotArm(float degree, int robot_arm_name);


	// member
public:
	// Camera Parameter
	int camera_amount;
	glm::vec4* camera_position;
	glm::vec4* look_at;
	glm::vec4* look_up;

	// Propeller Parameter
	Propeller propeller[NumPropeller];
	PropellerVector motionVector;
	// ROV Control Parameter
	float motionPower[NumMotion] = { 0 };

	// LED
	int spotlight_amount;
	glm::vec4* light_position;
	glm::vec4* light_direction;
	Light* light;

	// Voxel Data
	//VoxelData vdata;

	// Render object
	Model* main_ROV_model;
	Model* ball;
	RobotArmObject* robotarmobject;	// 7 object

	// PID Parameter
	int PID_amount;
	bool position_PID_worked[3];
	MyPID* position_PID[3];
	bool orientation_PID_worked[3];
	MyPID* orientation_PID[3];
	bool velocity_PID_worked[3];
	MyPID* velocity_PID[3];
	bool angularVelocity_PID_worked[3];
	MyPID* angularVelocity_PID[3];


	/* Bounding Box */
	OBB obb;												/* in body space */
	AABB aabb;												/* in world space */

	/* Physics Engine */
	/* Constant quantities */
	glm::vec3 size;											/* model size (m)*/
	double mass;											/* mass M */
	double volume;											/* volume V */
	glm::vec4 mass_center;									/* The position of mass center (BCS)	*/
	glm::vec4 volume_center;								/* The position of bouyancy center (BCS)*/
	glm::mat3 Inertia;										/*  */
	glm::mat3 InertiaInv;									/*  */

	/*Average qauntities*/
	double mass_Sum;
	double volume_Sum;
	glm::vec4 mass_center_Average;
	glm::vec4 buoyancy_center_Average;
	glm::mat3 Inertia_Average;								/*  */
	glm::mat3 InertiaInv_Average;							/*  */

	/* State variables */
	glm::vec3 total_displacement;							/*  */
	glm::vec3 total_rotate_radian;							/*  */

	/* Space transfer matrix */
	glm::mat4 BodyToMotionMatrix;							/* Body space to Motion space  (fixed)	*/
	glm::mat4 LinearSystemTranslateMatrix;					/* calculate by x(t)					*/
	glm::mat4 BodyToWorldTranslateMatrix_last;
	glm::mat4 BodyToWorldTranslateMatrix;					/* include linear system translation and translated by rotate */

	glm::mat4 RotateMatrix_last;							/* R(t - 1)				*/
	glm::mat4 RotateMatrix;									/* R(t)					*/
	glm::mat4 RotateMatrixInv_last;							/* R-1(t - 1)			*/
	glm::mat4 RotateMatrixInv;								/* R-1(t)				*/
	glm::mat4 modelMatrix_last;								/* M(t - 1)				*/
	glm::mat4 modelMatrix;									/* M(t)					*/

	// modelMatrix = MotionToWorldTranslateMatrix * RotateMatrix * BodyToMotionMatrix * glm::mat4(1.0f);
	// MotionToWorldTranslateMatrix = Translate(x)


	/* Physics Engine */
	// 1. Sum of force and torque.
	// 2. Calculate linear velocity and angular velocity.
	// 3. Use velocity and omega to update State veriavles.
	PhysicEngine PE;

};

