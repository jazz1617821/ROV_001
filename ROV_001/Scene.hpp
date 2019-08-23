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
#include "glm/gtc/type_ptr.hpp"
#include <list>
#include <cmath>

#include "Model.hpp"
#include "PerlinNoise.h"

#define LENGTH 60
#define SNOW_NUM 500

enum SceneKind { Sand, Rock, NumScene };
enum CollisionState{ Safe, Close, Contact };

struct POS {
	int x;
	int z;
};

class Terrain {
public:
	/*
	* terrain constructor
	* model_path : terrain model path (tecture and obj should put in the same folder)
	* ball_model_path : bounding sphere model path
	* size : bounding sphere size ( 0.3 is ok, position is fixed)
	* type : terrain type (SEND/ROCK)
	*/
	Terrain(const char* model_path, const char* ball_model_path, float size, int type) :type(type),size(size),model(model_path),ball(ball_model_path){
		nowPos.x = 5;
		nowPos.z = 5;
		terrain_position = glm::vec3(0.0, 0.0, 0.0);
	}

	/* get the height of terrain (x,z) position*/
	float height_send(double nx, double ny);
	float height_rock(double nx, double ny);

	/* check the range of rendering terrain */
	void check_position(glm::vec3 now_position);

	/*
	* rander the terrain (5*5 mesh)
	* now_position : viewer's position (you can put the ROV's position)
	*/
	void render(Shader shader, glm::vec3 now_position);

	/*
	* compute the collision between terrain and ROV
	* position : ROV's position
	* pitch : ROV's pitch angle
	* yaw : ROV's yaw angle
	* row : ROV's row angle
	* terrain_collision_size : scale the size of terrain's point
	* collosion_point_in_world_space : the world space position of collision point (put the empty glm::vec3, and it will return the position, if it detect the collision)
	*/
	CollisionState collision(glm::vec3 position, glm::mat4 model_matrix, glm::vec3 &collosion_point_in_world_space);

	/*
	* render the bounding sphere * 6
	* position : ROV's position
	* pitch : ROV's pitch angle
	* yaw : ROV's yaw angle
	* row : ROV's row angle
	*/
	void render_collision(glm::mat4 model_matrix, Shader shader);

	/*
	* translate the terrain position, but it only translate once
	* (x,y,z) is the displacement
	*/
	void translate_terrain_position(float x, float y, float z);

public:
	float getElasticity();

private:
	PerlinNoise pn;
	Model model;
	Model ball;
	POS nowPos;
	float size;
	glm::vec3 terrain_position;
	int type;
	const float Elasticity[NumScene] = { 1.8,2.0 };
};


struct SNOW {
	glm::vec3 position;
	GLuint VAO;
};

class MarineSnow {
public:
	MarineSnow() {
		this->velocity = 0.001;
		this->marineSnowTextureID = Create_Texture_Marine_Snow();
		this->init_marine_snow();
		//this->marineSnowVAO = generateFaceVAO();
	}
	GLint Create_Texture_Marine_Snow();
	void init_marine_snow();
	void snow_drop(glm::vec3 rov_position);
	void render_marine_snow(glm::vec3 rov_position, Shader shader);

private:
	PerlinNoise pn;
	unsigned char marine_snow[128][128][4];
	GLint marineSnowTextureID;
	GLuint marineSnowVAO;
	SNOW snow[SNOW_NUM];
	float velocity;
};