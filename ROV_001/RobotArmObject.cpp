#include "RobotArmObject.hpp"



RobotArmObject::RobotArmObject(string modelfilepath, string parameterfilepath)
{

	this->robotArmObjectModel = new Model(modelfilepath.c_str());
	this->ball = new Model("./Model/Scene/ball/ball.obj");

	offset_center_position = glm::vec3(0.0, 0.0, 0.0);
	offset_rotate_flag = false;
	offest_radian = glm::radians(0.0f);
	offest_andian_rotate_axis = glm::vec3(1.0, 0.0, 0.0);
	control_rotate_flag = false;
	control_radian_rotate_axis = glm::vec3(1.0, 0.0, 0.0);
	control_radian = glm::radians(0.0f);

	glm::vec3 obb_min, obb_max;
	//load paramter from file
	string file_str("empty string!");
	ifstream file;
	file.open(parameterfilepath);
	if (file.is_open()) {
		string onelineStr;
		while (getline(file, onelineStr)) {
			// Mass
			if (onelineStr.find("Mass ( kg ) = ") != string::npos) {
				int star = onelineStr.find("=");
				int end = onelineStr.find("\0");
				onelineStr = onelineStr.substr(star + 1, end - star - 1);
				this->mass = stof(onelineStr.c_str());
				continue;
			}
			// Mass center
			if (onelineStr.find("Mass center offset ( m ) = ") != string::npos) {
				int star = onelineStr.find("=");
				int end = onelineStr.find("\0");
				onelineStr = onelineStr.substr(star + 1, end - star - 1);
				std::string::size_type sz = 0;
				std::string::size_type idx = 0;
				this->mass_center.x = stof(onelineStr.c_str(), &sz);
				idx += sz + 1;
				this->mass_center.y = stof(onelineStr.substr(idx), &sz);
				idx += sz + 1;
				this->mass_center.z = stof(onelineStr.substr(idx), &sz);
				this->mass_center.w = 1.0;
				continue;
			}
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
			// Inertia
			if (onelineStr.find("Inertia tensor ( kg * m^2 ) = ") != string::npos) {
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
			if (onelineStr.find("Volume center offset ( m ) = ") != string::npos) {
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
				continue;
			}
			// Offset center position
			if (onelineStr.find("Offset center position = ") != string::npos) {
				int star = onelineStr.find("=");
				int end = onelineStr.find("\0");
				onelineStr = onelineStr.substr(star + 1, end - star - 1);
				std::string::size_type sz;
				std::string::size_type idx = 0;
				offset_center_position.x = std::stof(onelineStr, &sz);
				idx += sz + 1;
				offset_center_position.y = std::stof(onelineStr.substr(idx), &sz);
				idx += sz + 1;
				offset_center_position.z = std::stof(onelineStr.substr(idx), &sz);
				continue;
			}
			// Offset rotate angle
			if (onelineStr.find("Offset rotate angle = ") != string::npos) {
				this->offset_rotate_flag = true;
				int star = onelineStr.find("=");
				int end = onelineStr.find("\0");
				onelineStr = onelineStr.substr(star + 1, end - star - 1);
				std::string::size_type sz;
				std::string::size_type idx = 0;
				offest_radian = glm::radians(std::stof(onelineStr, &sz));
				continue;
			}
			// Offset rotate axis
			if (onelineStr.find("Offset rotate axis = ") != string::npos) {
				int star = onelineStr.find("=");
				int end = onelineStr.find("\0");
				onelineStr = onelineStr.substr(star + 1, end - star - 1);
				std::string::size_type sz;
				std::string::size_type idx = 0;
				offest_andian_rotate_axis.x = std::stof(onelineStr, &sz);
				idx += sz + 1;
				offest_andian_rotate_axis.y = std::stof(onelineStr.substr(idx), &sz);
				idx += sz + 1;
				offest_andian_rotate_axis.z = std::stof(onelineStr.substr(idx), &sz);
				continue;
			}
			// Control rotate axis
			if (onelineStr.find("Control rotate axis = ") != string::npos) {
				this->control_rotate_flag = true;
				int star = onelineStr.find("=");
				int end = onelineStr.find("\0");
				onelineStr = onelineStr.substr(star + 1, end - star - 1);
				std::string::size_type sz;
				std::string::size_type idx = 0;
				control_radian_rotate_axis.x = std::stof(onelineStr, &sz);
				idx += sz + 1;
				control_radian_rotate_axis.y = std::stof(onelineStr.substr(idx), &sz);
				idx += sz + 1;
				control_radian_rotate_axis.z = std::stof(onelineStr.substr(idx), &sz);
				continue;
			}
			this->mass_center = this->mass_center + glm::vec4(obb_min, 0.0);
			this->volume_center = this->volume_center + glm::vec4(obb_min, 0.0);
			this->obb = OBB(obb_min, obb_max);
		}
		file.close();
	}

	offset_translate_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(offset_center_position.x, offset_center_position.y, offset_center_position.z));
	offset_rotate_matrix = glm::rotate(glm::mat4(1.0), offest_radian, offest_andian_rotate_axis);

}

RobotArmObject::~RobotArmObject()
{

}

void RobotArmObject::calculateBodyMatrix(glm::mat4 parentObjectModelMatrix)
{
	if (control_rotate_flag) {
		control_rotate_matirx = glm::rotate(glm::mat4(1.0), control_radian, control_radian_rotate_axis);
		if (offset_rotate_flag) {
			this->bodyMatrix = parentObjectModelMatrix * offset_translate_matrix * offset_rotate_matrix * control_rotate_matirx;
		}
		else {
			this->bodyMatrix = parentObjectModelMatrix * offset_translate_matrix * control_rotate_matirx;
		}
	}
	else {
		this->bodyMatrix = parentObjectModelMatrix * offset_translate_matrix;
	}
}

void RobotArmObject::calculateModelMatrix(glm::mat4 main_rov_modelmatrix)
{
	this->modelMatrix = main_rov_modelmatrix * this->bodyMatrix;
	calculateBoundingBox();
}

void RobotArmObject::calculateBoundingBox()
{
	this->obb.updateOBBLCS(this->modelMatrix);
	this->aabb = this->obb.transfer_2_AABB();
}

void RobotArmObject::drawPhysicsCenter(Shader * shader)
{
	glm::mat4 modelMatrix(1.0f);

	// mass_center
	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(this->mass_center));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.01, 0.01, 0.01));
	modelMatrix = this->modelMatrix * modelMatrix;
	shader->use();
	shader->setMat4("model", modelMatrix);
	shader->setVec3("color", glm::vec3(1.0, 0.0, 0.0));
	ball->Draw(*shader);

	// buoyancy(volume) center
	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(this->volume_center));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.01, 0.01, 0.01));
	modelMatrix = this->modelMatrix * modelMatrix;
	shader->use();
	shader->setMat4("model", modelMatrix);
	shader->setVec3("color", glm::vec3(0.0, 1.0, 0.0));
	ball->Draw(*shader);

}

void RobotArmObject::draw(Shader * shader)
{
	shader->use();
	shader->setMat4("model", this->modelMatrix);
	this->robotArmObjectModel->Draw(*shader);
}
