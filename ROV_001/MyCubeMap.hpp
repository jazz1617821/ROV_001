/*
 * ROV_001
 * Copyright (C) 2019 ROV Simulator
 * National Taiwan Ocean University
 * Department of Computer Science and Engineering
 * Computer Graphics Lab Chieh-Shih Chou, Shyh-Kuang Ueng
 * Copyright (C) 2019
 */
#pragma once
#include <iostream>
using namespace std;
#include "Shader.hpp"
#include "opencv2/opencv.hpp"

int loadCubeMap(const string* imagepath);

GLuint initialSkyBoxVAO();

void drawSkyBox(GLuint skyboxVAOID, GLuint cubemap_textureID);