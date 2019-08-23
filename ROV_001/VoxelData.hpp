/*
 * ROV_001
 * Copyright (C) 2019 ROV Simulator
 * National Taiwan Ocean University
 * Department of Computer Science and Engineering
 * Computer Graphics Lab Chieh-Shih Chou, Shyh-Kuang Ueng
 * Copyright (C) 2019
 */
#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;
class VoxelData
{
public:
	VoxelData();
	~VoxelData();

	void readData(string infofilename, string rawfilename);

	// Member
	int resolution[3];
	float voxelsize[3];				// (m)
	float* rawData;
};

