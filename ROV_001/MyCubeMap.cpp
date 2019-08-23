/*
 * ROV_001
 * Copyright (C) 2019 ROV Simulator
 * National Taiwan Ocean University
 * Department of Computer Science and Engineering
 * Lab 503 Chieh-Shih Chou, Shyh-Kuang Ueng
 * Copyright (C) 2019
 */
#include "MyCubeMap.hpp"

int loadCubeMap(const string* imagepath)
{
	GLuint cubemap;
	glGenTextures(1, &cubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);

	for (GLuint i = 0; i < 6; i++)
	{
		IplImage *image = cvLoadImage(imagepath[i].c_str(), CV_LOAD_IMAGE_UNCHANGED);

		if (!image)
		{
			std::cout << imagepath[i] << endl;
			puts("load image error!");
			return -1;
		}
		for (int i = 0; i < image->width; i++)
		{
			for (int j = 0; j < image->height; j++)
			{
				CvScalar s = cvGet2D(image, j, i);
				CvScalar a = cvScalar(s.val[2], s.val[1], s.val[0], s.val[3]);
				cvSet2D(image, j, i, a);
			}
		}
		if (image->nChannels == 4)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, image->width, image->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->imageData);
		else
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, image->width, image->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image->imageData);
		cvReleaseImage(&image);
	}
	//glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return cubemap;
}

GLuint initialSkyBoxVAO()
{
	float skyboxVertices[] = {
		// positions          
		-3.0f,  3.0f, -3.0f,
		-3.0f, -3.0f, -3.0f,
		 3.0f, -3.0f, -3.0f,
		 3.0f, -3.0f, -3.0f,
		 3.0f,  3.0f, -3.0f,
		-3.0f,  3.0f, -3.0f,

		-3.0f, -3.0f,  3.0f,
		-3.0f, -3.0f, -3.0f,
		-3.0f,  3.0f, -3.0f,
		-3.0f,  3.0f, -3.0f,
		-3.0f,  3.0f,  3.0f,
		-3.0f, -3.0f,  3.0f,

		 3.0f, -3.0f, -3.0f,
		 3.0f, -3.0f,  3.0f,
		 3.0f,  3.0f,  3.0f,
		 3.0f,  3.0f,  3.0f,
		 3.0f,  3.0f, -3.0f,
		 3.0f, -3.0f, -3.0f,

		-3.0f, -3.0f,  3.0f,
		-3.0f,  3.0f,  3.0f,
		 3.0f,  3.0f,  3.0f,
		 3.0f,  3.0f,  3.0f,
		 3.0f, -3.0f,  3.0f,
		-3.0f, -3.0f,  3.0f,

		-3.0f,  3.0f, -3.0f,
		 3.0f,  3.0f, -3.0f,
		 3.0f,  3.0f,  3.0f,
		 3.0f,  3.0f,  3.0f,
		-3.0f,  3.0f,  3.0f,
		-3.0f,  3.0f, -3.0f,

		-3.0f, -3.0f, -3.0f,
		-3.0f, -3.0f,  3.0f,
		 3.0f, -3.0f, -3.0f,
		 3.0f, -3.0f, -3.0f,
		-3.0f, -3.0f,  3.0f,
		 3.0f, -3.0f,  3.0f
	};

	// skybox VAO
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	return skyboxVAO;
}

void drawSkyBox(GLuint skyboxVAOID, GLuint cubemap_textureID)
{
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	glBindVertexArray(skyboxVAOID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_textureID);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS); // set depth function back to default
}