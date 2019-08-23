/*
 * ROV_001
 * Copyright (C) 2019 ROV Simulator
 * National Taiwan Ocean University
 * Department of Computer Science and Engineering
 * Computer Graphics Lab Chieh-Shih Chou, Shyh-Kuang Ueng
 * Copyright (C) 2019
 */
#include "ROV.hpp"

 // main rov
string objfilepath = "./Model/ROV/ROV.obj";
string parameterfilepath = "./Model/ROV/ROVParameter.txt";
string infofilepath = "./Model/ROV/ROV.inf";
string rawfilepath = "./Model/ROV/ROV.raw";
// robot arm
string robotArmModelFilepath[] =
{
	"./Model/ROV/Robot_Arm_Frame_Structure.obj",
	"./Model/ROV/Arm_Control_Joint_1.obj",
	"./Model/ROV/Arm_1.obj",
	"./Model/ROV/Arm_2.obj",
	"./Model/ROV/Arm_Control_Joint_2.obj",
	"./Model/ROV/Paw_1.obj",
	"./Model/ROV/Paw_2.obj"
};

string robotArmParameterFilepath[] =
{
	"./Model/ROV/Robot_Arm_Frame_Structure.txt",
	"./Model/ROV/Arm_Control_Joint_1.txt",
	"./Model/ROV/Arm_1.txt",
	"./Model/ROV/Arm_2.txt",
	"./Model/ROV/Arm_Control_Joint_2.txt",
	"./Model/ROV/Paw_1.txt",
	"./Model/ROV/Paw_2.txt"
};

ROV::ROV()
{
	this->main_ROV_model = new Model(objfilepath.c_str());
	this->ball = new Model("./Model/Scene/ball/ball.obj");

	if (loadParameter() == false) {

		/* initial quatities value */
		/* Constant quatities */
		aabb.min = glm::vec4(-0.350, -0.240, -0.570, 1.0);			// model min bound in body space
		aabb.max = glm::vec4(0.350, 0.310, 0.575, 1.0);				// model max bound in body space
		mass = 132.030445439999994;		// kg 
		size = glm::vec3(0.7, 0.49, 1.15);
		volume = 0.136714976000000;		// m^3
		Inertia = glm::mat3(													// kg * m^2
			14.518151093778201, -3.720718430291108, -6.575923486115306,
			-3.720718430291108, 17.503123680351607, -4.507825809255711,
			-6.575923486115306, -4.507825809255711, 9.386730312180672
		);
		// glm is colume major
		// so it would be
		// xx yx zx
		// xy yy zy
		// xz yz zz
		//


		/* State variables */
		mass_center = glm::vec4(				// mass center = min bound + mass center offset
			aabb.min.x + 0.358144117027076,
			aabb.min.y + 0.189492088947120,
			aabb.min.z + 0.571756626934147,
			1.0
		);
		volume_center = glm::vec4(			// mass center = min bound + volume center offset
			aabb.min.x + 0.356465753027320,
			aabb.min.y + 0.324143911943868,
			aabb.min.z + 0.563993352052024,
			1.0
		);
	}

	initialRobotArm();

	calculateSumPhysicsQuantities();
	calculateAveragePhysicsCenter();
	calculateAverageInertia();
	calculatePropellerVector();

	PE.initialize(this->mass_Sum, this->mass_center_Average, this->Inertia_Average, this->InertiaInv_Average);

	total_displacement = glm::vec3(0.0, -5.0, 0.0);
	total_rotate_radian = glm::vec3(0.0, 0.0, 0.0);

	/* Space transfer matrix */
	BodyToMotionMatrix = glm::translate(
		glm::mat4(1.0f),
		glm::vec3(-mass_center_Average.x, -mass_center_Average.y, -mass_center_Average.z)
	);

	/* Rotation Matrix */
	RotateMatrix = glm::mat4(1.0f);
	RotateMatrix = glm::rotate(
		glm::mat4(1.0f),
		PE.r.x,
		glm::vec3(1, 0, 0)
	);
	RotateMatrix = glm::rotate(
		RotateMatrix,
		PE.r.y,
		glm::vec3(0, 1, 0)
	);
	RotateMatrix = glm::rotate(
		RotateMatrix,
		PE.r.z,
		glm::vec3(0, 0, 1)
	);
	RotateMatrix_last = RotateMatrix;
	RotateMatrixInv = glm::inverse(RotateMatrix);						/* Define by PE.r. When PE.r changed and so does this.*/
	RotateMatrixInv_last = RotateMatrixInv;
	total_rotate_radian = glm::vec3(0.0f);

	LinearSystemTranslateMatrix = glm::mat4(1.0);
	BodyToWorldTranslateMatrix = glm::translate(glm::mat4(1.0f), total_displacement);
	modelMatrix = BodyToWorldTranslateMatrix * RotateMatrix * glm::mat4(1.0f);
	modelMatrix_last = modelMatrix;
	calculateBoundingBox();

	// set LED
	light = new Light[spotlight_amount];
	for (int i = 0; i < spotlight_amount; i++) {
		light[i] = Light(SPOTLIGHT);
		light[i].setAmbient(glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
		light[i].setDiffuse(glm::vec4(0.7f, 0.7f, 0.7f, 1.0f));
		light[i].setSpecular(glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
		light[i].setPosition(light_position[i]);
		light[i].setDirection(light_direction[i]);
		light[i].setCutoffAngle(25.0);
		light[i].setExponent(1.0);
	}


	initialPID();
}

ROV::~ROV()
{
	free(this->camera_position);
	free(this->look_at);
	free(this->look_up);
	free(this->light_position);
	free(this->light_direction);
	delete[] this->light;
	delete main_ROV_model;
	delete ball;
	for (int i = 0; i < 3; i++) {
		delete this->position_PID[i];
		delete this->orientation_PID[i];
		delete this->velocity_PID[i];
		delete this->angularVelocity_PID[i];
	}
}

bool ROV::loadParameter()
{
	glm::vec3 obb_min, obb_max;
	//load paramter from file
	string file_str("empty string!");
	ifstream file;
	file.open(parameterfilepath);
	if (file.is_open()) {
		string onelineStr;
		while (getline(file, onelineStr)) {
			// get mass center
			if (onelineStr.find("Mass center in body space(m) = ") != string::npos) {
				int star = onelineStr.find("=");
				int end = onelineStr.find("\0");
				onelineStr = onelineStr.substr(star + 1, end - star - 1);
				std::string::size_type sz;
				std::string::size_type idx = 0;
				mass_center[0] = stof(onelineStr.c_str(), &sz);
				idx += sz + 1;
				mass_center[1] = stof(onelineStr.substr(idx), &sz);
				idx += sz + 1;
				mass_center[2] = stof(onelineStr.substr(idx), &sz);
				mass_center[3] = 1.0;
				continue;
			}
			// AABB
			// min bound
			if (onelineStr.find("Min bound point(AABB min point) in body space (m) = ") != string::npos) {
				int star = onelineStr.find("=");
				int end = onelineStr.find("\0");
				onelineStr = onelineStr.substr(star + 1, end - star - 1);
				std::string::size_type sz;
				std::string::size_type idx = 0;
				obb_min.x = stof(onelineStr.c_str(), &sz);
				idx += sz + 1;
				obb_min.y = stof(onelineStr.substr(idx), &sz);
				idx += sz + 1;
				obb_min.z = stof(onelineStr.substr(idx), &sz);
				continue;
			}
			// max bound
			if (onelineStr.find("Max bound point(AABB max point) in body space (m) = ") != string::npos) {
				int star = onelineStr.find("=");
				int end = onelineStr.find("\0");
				onelineStr = onelineStr.substr(star + 1, end - star - 1);
				std::string::size_type sz;
				std::string::size_type idx = 0;
				obb_max.x = stof(onelineStr.c_str(), &sz);
				idx += sz + 1;
				obb_max.y = stof(onelineStr.substr(idx), &sz);
				idx += sz + 1;
				obb_max.z = stof(onelineStr.substr(idx), &sz);
				continue;
			}
			this->obb = OBB(obb_min, obb_max);
			// Mass
			if (onelineStr.find("Mass(kg) = ") != string::npos) {
				int star = onelineStr.find("=");
				int end = onelineStr.find("\0");
				onelineStr = onelineStr.substr(star + 1, end - star - 1);
				mass = stof(onelineStr.c_str());
				continue;
			}
			// Size
			if (onelineStr.find("Size(m) = ") != string::npos) {
				int star = onelineStr.find("=");
				int end = onelineStr.find("\0");
				onelineStr = onelineStr.substr(star + 1, end - star - 1);
				std::string::size_type sz;
				std::string::size_type idx = 0;
				size.x = stof(onelineStr.c_str(), &sz);
				idx += sz + 1;
				size.y = stof(onelineStr.substr(idx), &sz);
				idx += sz + 1;
				size.z = stof(onelineStr.substr(idx), &sz);
				continue;
			}
			// Inertia
			if (onelineStr.find("Inertia tensor ( kg * m^2 ) in motion space = ") != string::npos) {
				int star = onelineStr.find("=");
				int end = onelineStr.find("\0");
				onelineStr = onelineStr.substr(star + 1, end - star - 1);
				std::string::size_type sz;
				std::string::size_type idx = 0;
				float xx, xy, xz, yx, yy, yz, zx, zy, zz;
				// vector 1
				xx = std::stof(onelineStr, &sz);
				idx += sz + 1;
				xy = std::stof(onelineStr.substr(idx), &sz);
				idx += sz + 1;
				xz = std::stof(onelineStr.substr(idx), &sz);
				// vector 2
				getline(file, onelineStr);
				idx = 0;
				yx = std::stof(onelineStr, &sz);
				idx += sz + 1;
				yy = std::stof(onelineStr.substr(idx), &sz);
				idx += sz + 1;
				yz = std::stof(onelineStr.substr(idx), &sz);
				// vector 3
				getline(file, onelineStr);
				idx = 0;
				zx = std::stof(onelineStr, &sz);
				idx += sz + 1;
				zy = std::stof(onelineStr.substr(idx), &sz);
				idx += sz + 1;
				zz = std::stof(onelineStr.substr(idx), &sz);

				Inertia = glm::mat3(xx, xy, xz, yx, yy, yz, zx, zy, zz);

				continue;
			}
			// Volume 
			if (onelineStr.find("Volume ( m^3 ) = ") != string::npos) {
				int star = onelineStr.find("=");
				int end = onelineStr.find("\0");
				onelineStr = onelineStr.substr(star + 1, end - star - 1);
				volume = std::stof(onelineStr);
				continue;
			}
			// Volume center (Bouyancy center)
			if (onelineStr.find("Volume center in body space(m) = ") != string::npos) {
				int star = onelineStr.find("=");
				int end = onelineStr.find("\0");
				onelineStr = onelineStr.substr(star + 1, end - star - 1);
				std::string::size_type sz;
				std::string::size_type idx = 0;
				volume_center[0] = std::stof(onelineStr, &sz);
				idx += sz + 1;
				volume_center[1] = std::stof(onelineStr.substr(idx), &sz);
				idx += sz + 1;
				volume_center[2] = std::stof(onelineStr.substr(idx), &sz);
				volume_center[3] = 1.0;
			}
			// Propeller
			if (onelineStr.find("propeller amount = ") != string::npos) {
				int star = onelineStr.find("=");
				int end = onelineStr.find("\0");
				onelineStr = onelineStr.substr(star + 1, end - star - 1);
				std::stof(onelineStr);
				for (int i = 0; i < NumPropeller; i++) {
					std::string::size_type sz;
					std::string::size_type idx = 0;
					glm::vec4 position;
					glm::vec4 force_vector;
					getline(file, onelineStr);
					int star = onelineStr.find("=");
					int end = onelineStr.find("\0");
					onelineStr = onelineStr.substr(star + 1, end - star - 1);
					position.x = std::stof(onelineStr, &sz);
					idx += sz + 1;
					position.y = std::stof(onelineStr.substr(idx), &sz);
					idx += sz + 1;
					position.z = std::stof(onelineStr.substr(idx), &sz);
					position.w = 1.0;
					idx += sz + 1;
					star = onelineStr.find("=");
					end = onelineStr.find("\0");
					idx = 0;
					onelineStr = onelineStr.substr(star + 1, end - star - 1);
					force_vector.x = std::stof(onelineStr.substr(idx), &sz);
					idx += sz + 1;
					force_vector.y = std::stof(onelineStr.substr(idx), &sz);
					idx += sz + 1;
					force_vector.z = std::stof(onelineStr.substr(idx), &sz);
					force_vector.w = 0;
					propeller[i].initialParameter(position, force_vector);
				}
			}
			// Camera
			if (onelineStr.find("camera amount = ") != string::npos) {
				int star = onelineStr.find("=");
				int end = onelineStr.find("\0");
				onelineStr = onelineStr.substr(star + 1, end - star - 1);
				camera_amount = std::stof(onelineStr);
				camera_position = (glm::vec4*)calloc(camera_amount, sizeof(glm::vec4));
				look_at = (glm::vec4*)calloc(camera_amount, sizeof(glm::vec4));
				look_up = (glm::vec4*)calloc(camera_amount, sizeof(glm::vec4));
				for (int i = 0; i < camera_amount; i++) {
					// set camera position and look up vector 
					std::string::size_type sz;
					std::string::size_type idx = 0;
					getline(file, onelineStr);
					int star = onelineStr.find("=");
					int end = onelineStr.find("\0");
					onelineStr = onelineStr.substr(star + 1, end - star - 1);
					camera_position[i].x = std::stof(onelineStr, &sz);
					idx += sz + 1;
					camera_position[i].y = std::stof(onelineStr.substr(idx), &sz);
					idx += sz + 1;
					camera_position[i].z = std::stof(onelineStr.substr(idx), &sz);
					camera_position[i].w = 1.0;
					idx += sz + 1;
					star = onelineStr.find("=");
					end = onelineStr.find("\0");
					idx = 0;
					onelineStr = onelineStr.substr(star + 1, end - star - 1);
					look_at[i].x = std::stof(onelineStr.substr(idx), &sz);
					idx += sz + 1;
					look_at[i].y = std::stof(onelineStr.substr(idx), &sz);
					idx += sz + 1;
					look_at[i].z = std::stof(onelineStr.substr(idx), &sz);
					look_at[i].w = 0.0;
					idx += sz + 1;
					star = onelineStr.find("=");
					end = onelineStr.find("\0");
					onelineStr = onelineStr.substr(star + 1, end - star - 1);
					idx = 0;
					look_up[i].x = std::stof(onelineStr.substr(idx), &sz);
					idx += sz + 1;
					look_up[i].y = std::stof(onelineStr.substr(idx), &sz);
					idx += sz + 1;
					look_up[i].z = std::stof(onelineStr.substr(idx), &sz);
					look_up[i].w = 0.0;
				}
			}
			// LED
			if (onelineStr.find("spotlights amount = ") != string::npos) {
				int star = onelineStr.find("=");
				int end = onelineStr.find("\0");
				onelineStr = onelineStr.substr(star + 1, end - star - 1);
				spotlight_amount = std::stof(onelineStr);
				light_position = (glm::vec4*)calloc(spotlight_amount, sizeof(glm::vec4));
				light_direction = (glm::vec4*)calloc(spotlight_amount, sizeof(glm::vec4));
				std::string::size_type sz;
				std::string::size_type idx = 0;
				for (int i = 0; i < spotlight_amount; i++) {
					// set spotlight position and direction 
					std::string::size_type sz;
					std::string::size_type idx = 0;
					getline(file, onelineStr);
					int star = onelineStr.find("=");
					int end = onelineStr.find("\0");
					onelineStr = onelineStr.substr(star + 1, end - star - 1);
					light_position[i].x = std::stof(onelineStr, &sz);
					idx += sz + 1;
					light_position[i].y = std::stof(onelineStr.substr(idx), &sz);
					idx += sz + 1;
					light_position[i].z = std::stof(onelineStr.substr(idx), &sz);
					light_position[i].w = 1.0;
					star = onelineStr.find("=");
					end = onelineStr.find("\0");
					idx = 0;
					onelineStr = onelineStr.substr(star + 1, end - star - 1);
					light_direction[i].x = std::stof(onelineStr.substr(idx), &sz);
					idx += sz + 1;
					light_direction[i].y = std::stof(onelineStr.substr(idx), &sz);
					idx += sz + 1;
					light_direction[i].z = std::stof(onelineStr.substr(idx), &sz);
					light_direction[i].w = 0.0;
				}
			}
		}

		file.close();
		return true;
	}
	return false;
}

void ROV::initialPID()
{
	PID_amount = 12;

	position_PID_worked[X] = position_PID_worked[Y] = position_PID_worked[Z] = false;
	orientation_PID_worked[X] = orientation_PID_worked[Y] = orientation_PID_worked[Z] = false;
	velocity_PID_worked[X] = velocity_PID_worked[Y] = velocity_PID_worked[Z] = false;
	angularVelocity_PID_worked[X] = angularVelocity_PID_worked[Y] = angularVelocity_PID_worked[Z] = false;

	position_PID[X] = new MyPID(2.0, 1.0, 0.0);
	position_PID[Y] = new MyPID(6.0, 3.0, 1.0);
	position_PID[Z] = new MyPID(2.0, 1.0, 0.0);

	orientation_PID[X] = new MyPID(0.05, 0.001, 0.1);
	orientation_PID[Y] = new MyPID(0.05, 0.0, 0.0);
	orientation_PID[Z] = new MyPID(0.05, 0.001, 0.1);

	velocity_PID[X] = new MyPID(2.0, 1.0, 0.0);
	velocity_PID[Y] = new MyPID(5.0, 4.0, 0.0);
	velocity_PID[Z] = new MyPID(2.0, 1.0, 0.0);

	angularVelocity_PID[X] = new MyPID(1, 0.0, 0.0);
	angularVelocity_PID[Y] = new MyPID(2, 2, 0.0);
	angularVelocity_PID[Z] = new MyPID(2, 0.01, 0.0);
}

void ROV::initialRobotArm()
{
	/* robot arm */
	robotarmobject = new RobotArmObject[NumRobotArmName]{
		{ robotArmModelFilepath[RobotArmFrameStructure], robotArmParameterFilepath[RobotArmFrameStructure] },
		{ robotArmModelFilepath[ControlJoint1], robotArmParameterFilepath[ControlJoint1] },
		{ robotArmModelFilepath[Arm1], robotArmParameterFilepath[Arm1] },
		{ robotArmModelFilepath[Arm2], robotArmParameterFilepath[Arm2] },
		{ robotArmModelFilepath[ControlJoint2], robotArmParameterFilepath[ControlJoint2] },
		{ robotArmModelFilepath[Paw1], robotArmParameterFilepath[Paw1] },
		{ robotArmModelFilepath[Paw2], robotArmParameterFilepath[Paw2] }
	};

	///*------------------------- Robot Arm ---------------------------*/
	calculateRobotArmTransferMatrix();
	///*---------------------------------------------------------------*/
}


void ROV::drawMainROV(Shader * shader)
{
	shader->use();
	shader->setMat4("model", this->modelMatrix);
	main_ROV_model->Draw(*shader);
}

void ROV::drawRobotArm(Shader * shader)
{
	for (int i = 0; i < NumRobotArmName; i++)
	{
		this->robotarmobject[i].draw(shader);
	}
}

void ROV::drawBoundingBox(Shader * shader, int mode)
{
	if (mode == 0)		//draw AABB
	{
		this->aabb.drawBoundingBox(shader);
		for (int i = 0; i < NumRobotArmName; i++)
		{
			this->robotarmobject[i].aabb.drawBoundingBox(shader);
		}
	}
	else if(mode == 1)	//draw OBB
	{
		this->obb.drawBoundingBox(shader);
		for (int i = 0; i < NumRobotArmName;i++) 
		{
			this->robotarmobject[i].obb.drawBoundingBox(shader);
		}
		//this->robotarmobject[ControlJoint1].obb.drawBoundingBox(shader);
	}
	else if(mode == 2)				// draw both
	{
		this->aabb.drawBoundingBox(shader);
		this->obb.drawBoundingBox(shader);
		for (int i = 0; i < NumRobotArmName; i++)
		{
			this->robotarmobject[i].aabb.drawBoundingBox(shader);
		}
		for (int i = 0; i < NumRobotArmName; i++)
		{
			this->robotarmobject[i].obb.drawBoundingBox(shader);
		}
	}
}

void ROV::drawPhysicsCenter(Shader * shader)
{
	//this->calculateAveragePhysicsCenter();
	//glDisable(GL_DEPTH_TEST);
	glm::mat4 modelMatrix(1.0);
	// draw main rov mass center
	/*modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(this->mass_center));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.01, 0.01, 0.01));
	modelMatrix = this->modelMatrix * modelMatrix;
	shader->use();
	shader->setMat4("model", modelMatrix);
	shader->setVec3("color", glm::vec3(1.0, 0.0, 0.0));
	ball->Draw(*shader);*/
	// draw main rov buoyancy center
	/*modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(this->volume_center));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.01, 0.01, 0.01));
	modelMatrix = this->modelMatrix * modelMatrix;
	shader->use();
	shader->setMat4("model", modelMatrix);
	shader->setVec3("color", glm::vec3(0.0, 1.0, 0.0));
	ball->Draw(*shader);*/
	// draw average mass center
	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(this->mass_center_Average));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.01, 0.01, 0.01));
	modelMatrix = this->modelMatrix * modelMatrix;
	shader->use();
	shader->setMat4("model", modelMatrix);
	shader->setVec3("color", glm::vec3(1.0, 1.0, 0.0));
	ball->Draw(*shader);
	// draw average bouyancy(volume) center
	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(this->buoyancy_center_Average));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.01, 0.01, 0.01));
	modelMatrix = this->modelMatrix * modelMatrix;
	shader->use();
	shader->setMat4("model", modelMatrix);
	shader->setVec3("color", glm::vec3(0.0, 1.0, 1.0));
	ball->Draw(*shader);

	// draw robot arm object physics center
	/*for (int i = 0; i < NumRobotArmName; i++)
	{
		this->robotarmobject[i].drawPhysicsCenter(shader);
	}
	*/
	//glEnable(GL_DEPTH_TEST);
}

void ROV::clearMotionPower()
{
	for (int i = Heave; i < NumMotion; i++) {
		this->motionPower[i] = 0.0;
	}
}

void ROV::runPID(glm::vec3 global_target_position, glm::vec3 target_orientation, glm::vec3 target_velocity, glm::vec3 target_angularVelocity, float time_interval)
{
	// position PID
	if (this->position_PID_worked[X] || this->position_PID_worked[Y] || this->position_PID_worked[Z]) {
		glm::vec3 global_position_error(0.0f);
		if (this->position_PID_worked[X]) {
			global_position_error.x = global_target_position.x - this->total_displacement.x;
		}
		if (this->position_PID_worked[Y]) {
			global_position_error.y = global_target_position.y - this->total_displacement.y;
		}
		if (this->position_PID_worked[Z]) {
			global_position_error.z = global_target_position.z - this->total_displacement.z;
		}
		glm::vec3 motion_position_error = glm::mat3(this->RotateMatrixInv) * global_position_error;
		motionPower[Sway] = this->position_PID[X]->processesPID(motion_position_error.x, time_interval);
		motionPower[Heave] = this->position_PID[Y]->processesPID(motion_position_error.y, time_interval);
		motionPower[Surge] = this->position_PID[Z]->processesPID(motion_position_error.z, time_interval);
	}
	// orientation PID
	if (this->orientation_PID_worked[X] || this->orientation_PID_worked[Y] || this->orientation_PID_worked[Z]) {
		glm::vec3 orientation_error(0.0f);
		glm::vec3 total_rotate_degree = glm::degrees(total_rotate_radian);
		if (this->orientation_PID_worked[X]) {
			orientation_error.x = target_orientation.x - total_rotate_degree.x;
		}
		if (this->orientation_PID_worked[Y]) {
			orientation_error.y = target_orientation.y - total_rotate_degree.y;
		}
		if (this->orientation_PID_worked[Z]) {
			orientation_error.z = target_orientation.z - total_rotate_degree.z;
		}
		glm::vec3 motion_orientation_error = orientation_error;
		motionPower[Pitch] = this->orientation_PID[X]->processesPID(motion_orientation_error.x, time_interval);
		motionPower[Yaw] = -this->orientation_PID[Y]->processesPID(motion_orientation_error.y, time_interval);
		motionPower[Roll] = this->orientation_PID[Z]->processesPID(motion_orientation_error.z, time_interval);
	}
	// velocity PID
	if (this->velocity_PID_worked[X] || this->velocity_PID_worked[Y] || this->velocity_PID_worked[Z]) {
		glm::vec3 velocity_error(0.0f);
		glm::vec3 motion_velocity = glm::mat3(this->RotateMatrixInv) * PE.v;
		if (this->velocity_PID_worked[X]) {
			velocity_error.x = target_velocity.x - motion_velocity.x;
			motionPower[Sway] = this->velocity_PID[X]->processesPID(velocity_error.x, time_interval);
		}
		if (this->velocity_PID_worked[Y]) {
			velocity_error.y = target_velocity.y - motion_velocity.y;
			motionPower[Heave] = this->velocity_PID[Y]->processesPID(velocity_error.y, time_interval);
		}
		if (this->velocity_PID_worked[Z]) {
			velocity_error.z = target_velocity.z - motion_velocity.z;
			motionPower[Surge] = this->velocity_PID[Z]->processesPID(velocity_error.z, time_interval);
		}
	}
	// angular velocity PID
	if (this->angularVelocity_PID_worked[X] || this->angularVelocity_PID_worked[Y] || this->angularVelocity_PID_worked[Z]) {
		glm::vec3 angular_velocity_error(0.0f);
		glm::vec3 motion_angular_velocity = PE.omega;
		if (this->angularVelocity_PID_worked[X]) {
			angular_velocity_error.x = target_angularVelocity.x - motion_angular_velocity.x;
			motionPower[Pitch] = -this->angularVelocity_PID[X]->processesPID(angular_velocity_error.x, time_interval);
		}
		if (this->angularVelocity_PID_worked[Y]) {
			angular_velocity_error.y = target_angularVelocity.y - motion_angular_velocity.y;
			motionPower[Yaw] = this->angularVelocity_PID[Y]->processesPID(angular_velocity_error.y, time_interval);
		}
		if (this->angularVelocity_PID_worked[Z]) {
			angular_velocity_error.z = target_angularVelocity.z - motion_angular_velocity.z;
			motionPower[Roll] = this->angularVelocity_PID[Z]->processesPID(angular_velocity_error.z, time_interval);
		}
	}

}

void ROV::addHydroStaticForce()
{
	this->PE.addGlobalForce(
		glm::vec4(0.0, -1.0, 0.0, 0.0) * (float)(this->mass_Sum * 9.8),
		this->mass_center_Average,
		this->RotateMatrixInv
	);
	// add bouyancy
	this->PE.addGlobalForce(
		glm::vec4(0.0, 1.0, 0.0, 0.0) * (float)(this->volume_Sum * 1030 * 9.8),
		this->buoyancy_center_Average,
		this->RotateMatrixInv
	);
}

glm::vec3 ROV::addDragForce()
{
	// add Drag
	// linear Drag
	glm::vec3 drag = this->PE.calculateDragForce(this->size, this->RotateMatrixInv);
	// angular Drag
	this->PE.calculateResistanceForce(this->size, this->mass_center_Average, this->RotateMatrix);

	return drag;
}

void ROV::addPropellerForce()
{
	for (int i = SV; i < NumPropeller; i++) {
		this->PE.addLocalForce(
			propeller[i].calculateForceVector(this->motionVector.getVectorComponent(i)),
			propeller[i].force_position,
			this->RotateMatrix
		);
	}
}

void ROV::calculateTransferMatrix()
{
	BodyToMotionMatrix = glm::translate(
		glm::mat4(1.0f),
		glm::vec3(-mass_center_Average.x, -mass_center_Average.y, -mass_center_Average.z)
	);

	/* Translate Matrix */
	// translate by linear system
	LinearSystemTranslateMatrix = glm::translate(
		glm::mat4(1.0f),
		PE.x
	);

	/* Rotate matrix */
	glm::mat4 newRotate = glm::rotate(
		glm::mat4(1.0f),
		PE.r.x,
		glm::vec3(1, 0, 0)
	);
	newRotate = glm::rotate(
		newRotate,
		PE.r.y,
		glm::vec3(0, 1, 0)
	);
	newRotate = glm::rotate(
		newRotate,
		PE.r.z,
		glm::vec3(0, 0, 1)
	);
	RotateMatrix_last = RotateMatrix;
	RotateMatrix = RotateMatrix * newRotate;
	RotateMatrixInv_last = RotateMatrixInv;
	RotateMatrixInv = glm::inverse(RotateMatrix);
	calculateRotateAngle();

	// translate by rotate
	glm::vec3 BodyCenter_1_InMotion = BodyToMotionMatrix * glm::vec4(0.0, 0.0, 0.0, 1.0);
	glm::vec3 BodyCenter_2_InMotion = newRotate * BodyToMotionMatrix * glm::vec4(0.0, 0.0, 0.0, 1.0);
	glm::vec3 TranslateByRotate_InWorld = RotateMatrix_last * glm::vec4(BodyCenter_2_InMotion - BodyCenter_1_InMotion, 1.0);
	glm::mat4 TranslatedByRotateMatrix = glm::translate(glm::mat4(1.0f), TranslateByRotate_InWorld);
	BodyToWorldTranslateMatrix_last = BodyToWorldTranslateMatrix;
	BodyToWorldTranslateMatrix *= LinearSystemTranslateMatrix * TranslatedByRotateMatrix;
	calculateDisplacement();

	modelMatrix_last = modelMatrix;
	modelMatrix = BodyToWorldTranslateMatrix * RotateMatrix * glm::mat4(1.0f);
	calculateBoundingBox();

	/*------------ Robot Arm -------------*/
	calculateRobotArmTransferMatrix();
	/*------------------------------------*/
}

void ROV::calculateRobotArmTransferMatrix()
{
	glm::mat4 parentModelMatrix(1.0f);
	for (int i = 0; i < NumRobotArmName - 1; i++)
	{
		this->robotarmobject[i].calculateBodyMatrix(parentModelMatrix);
		this->robotarmobject[i].calculateModelMatrix(this->modelMatrix);
		parentModelMatrix = this->robotarmobject[i].bodyMatrix;
	}
	parentModelMatrix = this->robotarmobject[ControlJoint2].bodyMatrix;
	this->robotarmobject[Paw2].calculateModelMatrix(this->modelMatrix);
	this->robotarmobject[Paw2].calculateBodyMatrix(parentModelMatrix);
}

void ROV::calculateRotateAngle()
{
	glm::vec3 Xs = glm::mat3(RotateMatrix) * glm::vec3(1.0, 0.0, 0.0);
	glm::vec3 Ys = glm::mat3(RotateMatrix) * glm::vec3(0.0, 1.0, 0.0);
	glm::vec3 Zs = glm::mat3(RotateMatrix) * glm::vec3(0.0, 0.0, 1.0);

	glm::vec3 left, front, up;
	float pitch_rad, yaw_rad, roll_rad;

	// PITCH ANGLE
	front = glm::cross(Xs, glm::vec3(0.0, 1.0, 0.0));
	up = glm::cross(front, Xs);
	pitch_rad = acos(glm::dot(glm::normalize(up), glm::normalize(Ys)));
	if (Zs.y > 0) {
		total_rotate_radian.x = -pitch_rad;
	}
	else {
		total_rotate_radian.x = pitch_rad;
	}



	// YAW ANGLE
	left = glm::cross(Ys, glm::vec3(0.0, 0.0, 1.0));
	front = glm::cross(left, Ys);
	yaw_rad = acos(glm::dot(glm::normalize(front), glm::normalize(Zs)));
	if (Xs.z > 0) {
		total_rotate_radian.y = yaw_rad;
	}
	else {
		total_rotate_radian.y = -yaw_rad;
	}

	// ROW ANGLE
	left = glm::cross(glm::vec3(0.0, 1.0, 0.0), Zs);
	up = glm::cross(Zs, left);
	roll_rad = acos(glm::dot(glm::normalize(up), glm::normalize(Ys)));
	if (Xs.y > 0) {
		total_rotate_radian.z = roll_rad;
	}
	else {
		total_rotate_radian.z = -roll_rad;
	}
}

void ROV::calculateDisplacement()
{
	this->total_displacement.x = BodyToWorldTranslateMatrix[3][0];
	this->total_displacement.y = BodyToWorldTranslateMatrix[3][1];
	this->total_displacement.z = BodyToWorldTranslateMatrix[3][2];
}

void ROV::calculateBoundingBox()
{
	this->obb.updateOBBLCS(this->modelMatrix);
	this->aabb = this->obb.transfer_2_AABB();
}

void ROV::calculateSumPhysicsQuantities()
{
	this->mass_Sum = ((float)this->mass);
	this->volume_Sum = ((float)this->volume);

	for (int i = 0; i < NumRobotArmName; i++)
	{
		this->mass_Sum += ((float)this->robotarmobject[i].mass);
		this->volume_Sum += ((float)this->robotarmobject[i].volume);
	}
}

void ROV::calculateAveragePhysicsCenter()
{
	// Average Mass Center and Average Buoyancy Center
	glm::vec3 Mi = glm::vec3(0.0f);
	glm::vec3 Vi = glm::vec3(0.0f);
	Mi = ((float)this->mass) * glm::vec3(this->mass_center);
	Vi = ((float)this->volume) * glm::vec3(this->volume_center);

	for (int i = 0; i < NumRobotArmName; i++) 
	{
		Mi += ((float)this->robotarmobject[i].mass) * glm::vec3(this->robotarmobject[i].bodyMatrix * this->robotarmobject[i].mass_center);
		Vi += ((float)this->robotarmobject[i].volume) * glm::vec3(this->robotarmobject[i].bodyMatrix * this->robotarmobject[i].volume_center);
	}

	//this->mass_center_Average = this->mass_center;
	this->mass_center_Average = glm::vec4(Mi / (float)this->mass_Sum, 1.0);
	//this->mass_center_Average = glm::vec4(0.0, -0.085, 0.0, 1.0);

	//this->buoyancy_center_Average = this->volume_center;
	this->buoyancy_center_Average = glm::vec4(Vi / (float)this->volume_Sum, 1.0);
	//this->buoyancy_center_Average = glm::vec4(0.0, 0.015, 0.0, 1.0);

	glm::vec3 globla_mass_center = this->RotateMatrix * this->mass_center_Average;
	glm::vec3 globla_volume_center = this->RotateMatrix * this->buoyancy_center_Average;
//#ifdef _DEBUG
//	std::cout << "/--------------------------------------------------/" << std::endl
//		<< "global mass center: " << globla_mass_center.x << ", " << globla_mass_center.y << ", " << globla_mass_center.z << std::endl
//		<< "global volume center: " << globla_volume_center.x << ", " << globla_volume_center.y << ", " << globla_volume_center.z << std::endl;
//#endif
}

void ROV::calculateAverageInertia()
{
	Inertia_Average = this->Inertia;

	for (int i = 0; i < NumRobotArmName; i++) {
		Inertia_Average += glm::mat3(this->robotarmobject[i].bodyMatrix * glm::mat4(this->robotarmobject[i].Inertia));
	}

	InertiaInv_Average = glm::inverse(Inertia_Average);

//#ifdef _DEBUG
//	std::cout << "Inertia: " << glm::to_string(Inertia_Average) << std::endl
//		<< "/--------------------------------------------------/" << std::endl;
//#endif
}

void ROV::calculatePropellerVector()
{
	double b[NumPropeller];
	double **A, *x;
	double *test_b;

	double sum_b = 0;

	int n = NumPropeller;

	for (int i = 0; i < NumPropeller; i++) {
		b[i] = this->motionPower[i];
		sum_b += fabs(this->motionPower[i]);
	}

	if (sum_b >= 0.0001) {
		A = alloc_mtx(n);
		x = alloc_vec(n);
		for (int i = 0; i < NumPropeller; i++) {
			glm::vec3 r(this->propeller[i].force_position - this->mass_center_Average);
			glm::vec3 torque = glm::cross(r, glm::vec3(this->propeller[i].force_vector));
			A[0][i] = this->propeller[i].force_vector.x;
			A[1][i] = this->propeller[i].force_vector.y;
			A[2][i] = this->propeller[i].force_vector.z;
			A[3][i] = torque.x;
			A[4][i] = torque.y;
			A[5][i] = torque.z;
		}
		//fprintf(stderr, "A[][]=\n");
		//print_mtx(A, n);

		gauss_elm(A, b, n);
		back_substitute(A, x, b, n);

		//QR_solver(A, x, b[j], n);

		// test
		/*fprintf(stderr, "After Decomposition A[][]=\n");
		print_mtx(A, n);
		fprintf(stderr, "The ");
		fprintf(stderr, "solution x[]=\n");
		print_vec(x, n);
		for (int i = 0; i < NumPropeller; i++) {
			glm::vec3 r(this->propeller[i].force_position - this->mass_center_Average);
			glm::vec3 torque = glm::cross(r, glm::vec3(this->propeller[i].force_vector));
			A[0][i] = this->propeller[i].force_vector.x;
			A[1][i] = this->propeller[i].force_vector.y;
			A[2][i] = this->propeller[i].force_vector.z;
			A[3][i] = torque.x;
			A[4][i] = torque.y;
			A[5][i] = torque.z;
		}
		test_b = alloc_vec(6);
		mtx_vec_mult(test_b, A, x, 6);
		print_vec(test_b, n);*/
		

		motionVector = PropellerVector(x[0], x[1], x[2], x[3], x[4], x[5]);

		free(A);
		free(x);
	}
	else {
		motionVector = PropellerVector(0, 0, 0, 0, 0, 0);
	}

}

void ROV::resetLocation()
{
	this->PE.clear();
	this->PE.initialize(this->mass_Sum, this->mass_center_Average, this->Inertia_Average, this->InertiaInv_Average);

	RotateMatrix = glm::mat4(1.0f);
	RotateMatrix = glm::rotate(
		glm::mat4(1.0f),
		PE.r.x,
		glm::vec3(1, 0, 0)
	);
	RotateMatrix = glm::rotate(
		RotateMatrix,
		PE.r.y,
		glm::vec3(0, 1, 0)
	);
	RotateMatrix = glm::rotate(
		RotateMatrix,
		PE.r.z,
		glm::vec3(0, 0, 1)
	);
	RotateMatrixInv = glm::inverse(RotateMatrix);					/* Define by PE.r. When PE.r changed and so does this.*/
	total_rotate_radian = glm::vec3(0.0f);
	total_displacement = glm::vec3(0.0f);

	BodyToWorldTranslateMatrix = glm::mat4(1.0);
	modelMatrix = BodyToWorldTranslateMatrix * RotateMatrix * glm::mat4(1.0f);
}

void ROV::undoModelMatrix()
{
	this->PE.undoMotion();
	BodyToWorldTranslateMatrix = BodyToWorldTranslateMatrix_last;
	RotateMatrix = RotateMatrix_last;
	RotateMatrixInv = RotateMatrixInv_last;
	modelMatrix = modelMatrix_last;
}

void ROV::controlRobotArm(float degree, int robot_arm_name)
{
	switch (robot_arm_name) {
	case ControlJoint1:
		if(degree > 0.0f) {
			if (this->robotarmobject[ControlJoint1].control_radian < glm::radians(30.0f)) {
				this->robotarmobject[ControlJoint1].control_radian += glm::radians(degree);
			}
			else {
				this->robotarmobject[ControlJoint1].control_radian = glm::radians(30.0f);
			}
		}
		else {
			if (this->robotarmobject[ControlJoint1].control_radian > glm::radians(-45.0f)) {
				this->robotarmobject[ControlJoint1].control_radian += glm::radians(degree);
			}
			else {
				this->robotarmobject[ControlJoint1].control_radian = glm::radians(-45.0f);
			}
		}
		break;
	case Arm1:
		if (degree > 0.0f) {
			if (this->robotarmobject[Arm1].control_radian < glm::radians(90.0f)) {
				this->robotarmobject[Arm1].control_radian += glm::radians(degree);
			}
			else {
				this->robotarmobject[Arm1].control_radian = glm::radians(90.0f);
			}
		}
		else {
			if (this->robotarmobject[Arm1].control_radian > glm::radians(0.0f)) {
				this->robotarmobject[Arm1].control_radian += glm::radians(degree);
			}
			else {
				this->robotarmobject[Arm1].control_radian = glm::radians(0.0f);
			}
		}
		break;
	case Arm2:
		if (degree > 0.0f) {
			if (this->robotarmobject[Arm2].control_radian < glm::radians(0.0f)) {
				this->robotarmobject[Arm2].control_radian += glm::radians(degree);
			}			  
			else {		  
				this->robotarmobject[Arm2].control_radian = glm::radians(0.0f);
			}			  
		}				  
		else {			  
			if (this->robotarmobject[Arm2].control_radian > glm::radians(-135.0f)) {
				this->robotarmobject[Arm2].control_radian += glm::radians(degree);
			}			  
			else {		  
				this->robotarmobject[Arm2].control_radian = glm::radians(-135.0f);
			}
		}
		break;
	case ControlJoint2:
		if (degree > 0.0f) {
			if (this->robotarmobject[ControlJoint2].control_radian < glm::radians(180.0f)) {
				this->robotarmobject[ControlJoint2].control_radian += glm::radians(degree);
			}
			else {
				this->robotarmobject[ControlJoint2].control_radian = glm::radians(180.0f);
			}
		}
		else {
			if (this->robotarmobject[ControlJoint2].control_radian > glm::radians(-180.0f)) {
				this->robotarmobject[ControlJoint2].control_radian += glm::radians(degree);
			}
			else {
				this->robotarmobject[ControlJoint2].control_radian = glm::radians(-180.0f);
			}
		}
		break;
	case Paw1:
		if (degree > 0.0f) {
			if (this->robotarmobject[Paw1].control_radian < glm::radians(90.0f)) {
				this->robotarmobject[Paw1].control_radian += glm::radians(degree);
			}
			else {
				this->robotarmobject[Paw1].control_radian = glm::radians(90.0f);
			}
		}else {
			if (this->robotarmobject[Paw1].control_radian > glm::radians(0.0f)) {
				this->robotarmobject[Paw1].control_radian += glm::radians(degree);
			}
			else {
				this->robotarmobject[Paw1].control_radian = glm::radians(0.0f);
			}
		}
		break;
	case Paw2:
		if (degree < 0.0f) {
			if (this->robotarmobject[Paw2].control_radian > glm::radians(-90.0f)) {
				this->robotarmobject[Paw2].control_radian += glm::radians(degree);
			}
			else {
				this->robotarmobject[Paw2].control_radian = glm::radians(-90.0f);
			}
		}
		else {
			if (this->robotarmobject[Paw2].control_radian < glm::radians(0.0f)) {
				this->robotarmobject[Paw2].control_radian += glm::radians(degree);
			}
			else {
				this->robotarmobject[Paw2].control_radian = glm::radians(0.0f);
			}
		}
		break;
	default:
		break;
	}
}
