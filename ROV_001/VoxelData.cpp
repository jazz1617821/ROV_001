/*
 * ROV_001
 * Copyright (C) 2019 ROV Simulator
 * National Taiwan Ocean University
 * Department of Computer Science and Engineering
 * Computer Graphics Lab Chieh-Shih Chou, Shyh-Kuang Ueng
 * Copyright (C) 2019
 */
#include "VoxelData.hpp"



VoxelData::VoxelData()
{
	resolution[0] = resolution[1] = resolution[2] = 0.0;
	voxelsize[0] = voxelsize[1] = voxelsize[2] = 1.0;
}


VoxelData::~VoxelData()
{
	free(rawData);
}

void VoxelData::readData(string infofilename,string rawfilename)
{
	ifstream infofile;
	string onelineStr;

	infofile.open(infofilename);

	if(infofile){
		while (getline(infofile, onelineStr)) {
			size_t ptr = onelineStr.find("resolution=");
			if (ptr != string::npos) {
				int star = onelineStr.find("=");
				int end = onelineStr.find("\0");
				onelineStr = onelineStr.substr(star + 1, end - star - 1);
				istringstream token(onelineStr);
				string word;
				int i = 0;
				while(getline(token, word, 'x')) {
					resolution[i] = atoi(word.c_str());
					i++;
				}
			}
			ptr = onelineStr.find("voxel-size=");
			if (ptr != string::npos) {
				int star = onelineStr.find("=");
				int end = onelineStr.find("\0");
				onelineStr = onelineStr.substr(star + 1, end - star - 1);
				istringstream token(onelineStr);
				string word;
				int i = 0;
				while (getline(token, word, ':')) {
					voxelsize[i] = atof(word.c_str()) / 1000;
					i++;
				}
			}
		}
		infofile.close();
	}

	rawData = (float*)calloc(resolution[0] * resolution[1] * resolution[2], sizeof(float));
	
	FILE* rawfile = fopen(rawfilename.c_str(),"rb");

	fread(rawData, sizeof(float), resolution[0] * resolution[1] * resolution[2], rawfile);

	fclose(rawfile);

}
