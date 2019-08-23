#include "Scene.hpp"

float faceVertices[] = {
	-0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // top-right
	-0.5f, 0.5f, -0.5f, 1.0f, 1.0f, // top-left
	-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // bottom-left
	-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // bottom-left
	-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-right
	-0.5f, 0.5f, 0.5f, 1.0f, 0.0f, // top-right
};

float Terrain::height_send(double nx, double ny) {
	float h = 2.0 * pn.noise(0.5 * nx, 0.5 * ny, 1.0);
	return h;
}

float Terrain::height_rock(double nx, double ny) {
	float h = 1.00 * pn.noise(0.2 * nx, 0.2 * ny, 1.0)
			+ 0.75 * pn.noise(0.4 * nx, 0.4 * ny, 1.0)
			+ 0.50 * pn.noise(0.6 * nx, 0.6 * ny, 1.0)
			+ 0.25 * pn.noise(0.8 * nx, 0.8 * ny, 1.0);
			+ 0.13 * pn.noise(1.0 * nx, 1.0 * ny, 1.0);
	h = h / (1.00 + 0.75);

	h = pow(h, 3.00) * 3.0;

	return h;
}

void Terrain::check_position(glm::vec3 now_position) {
	nowPos.x = (now_position.x - terrain_position.x) / (LENGTH / 2.0) + 5;
	nowPos.z = (now_position.z - terrain_position.z) / (LENGTH / 2.0) + 5;
}

void Terrain::render(Shader shader, glm::vec3 now_position) {
	check_position(now_position);

	glm::mat4 model_matrix(1.0);
	model_matrix = glm::translate(model_matrix, terrain_position);
	int i, j;
	for (i = nowPos.x - 2; i <= nowPos.x + 2; i++) {
		for (j = nowPos.z - 2; j <= nowPos.z + 2; j++) {
			if (i * 10 + j >= 0 && i * 10 + j <= 99) {
				glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model_matrix));
				model.Draw_mesh(shader, i * 10 + j);
			}
		}
	}
}

CollisionState Terrain::collision(glm::vec3 position, glm::mat4 modelMat, glm::vec3 &collosion_point_in_world_space) {
	glm::mat4 ball(1.0);
	ball = modelMat;

	glm::vec3 ball1 = ball * glm::vec4(0.15, 0.0, 0.36, 1.0);
	glm::vec3 ball2 = ball * glm::vec4(0.15, 0.0, 0.0, 1.0);
	glm::vec3 ball3 = ball * glm::vec4(0.15, 0.0, -0.36, 1.0);
	glm::vec3 ball4 = ball * glm::vec4(-0.15, 0.0, 0.36, 1.0);
	glm::vec3 ball5 = ball * glm::vec4(-0.15, 0.0, 0.0, 1.0);
	glm::vec3 ball6 = ball * glm::vec4(-0.15, 0.0, -0.36, 1.0);
	
	float i, j, h;

	float distance = size;
	CollisionState state = CollisionState::Safe;

	for (i = (int)position.x - 1.0; i <= (int)position.x + 1.0; i += 0.5) {
		for (j = (int)position.z - 2.0; j <= (int)position.z + 2.0; j += 0.5) {
			if (type == Sand) {
				h = height_send(i - terrain_position.x, j - terrain_position.z) + terrain_position.y;
			}
			else if (type == Rock) {
				h = height_rock(i - terrain_position.x, j - terrain_position.z) + terrain_position.y;
			}
			if ((position.x - i)*(position.x - i) + (position.y - h)*(position.y - h) + (position.z - j)*(position.z - j) <= 2) {
				state = CollisionState::Close;
				if ((ball1.x - i)*(ball1.x - i) + (ball1.y - h)*(ball1.y - h) + (ball1.z - j)*(ball1.z - j) <= distance * distance ||
					(ball2.x - i)*(ball2.x - i) + (ball2.y - h)*(ball2.y - h) + (ball2.z - j)*(ball2.z - j) <= distance * distance ||
					(ball3.x - i)*(ball3.x - i) + (ball3.y - h)*(ball3.y - h) + (ball3.z - j)*(ball3.z - j) <= distance * distance ||
					(ball4.x - i)*(ball4.x - i) + (ball4.y - h)*(ball4.y - h) + (ball4.z - j)*(ball4.z - j) <= distance * distance ||
					(ball5.x - i)*(ball5.x - i) + (ball5.y - h)*(ball5.y - h) + (ball5.z - j)*(ball5.z - j) <= distance * distance ||
					(ball6.x - i)*(ball6.x - i) + (ball6.y - h)*(ball6.y - h) + (ball6.z - j)*(ball6.z - j) <= distance * distance) {
					collosion_point_in_world_space.x = i;
					collosion_point_in_world_space.y = h;
					collosion_point_in_world_space.z = j;
					return CollisionState::Contact;;
				}
			}
		}
	}

	return state;
}

void Terrain::render_collision(glm::mat4 modelMat, Shader shader) {
	glm::mat4 model_matrix(1.0);
	model_matrix = modelMat;

	glm::mat4 model_matrix1, model_matrix2, model_matrix3, model_matrix4, model_matrix5, model_matrix6;

	model_matrix1 = glm::translate(model_matrix, glm::vec3(0.15, 0.0, 0.36));
	model_matrix1 = glm::scale(model_matrix1, glm::vec3(size, size, size));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model_matrix1));
	ball.Draw(shader);

	model_matrix2 = glm::translate(model_matrix, glm::vec3(0.15, 0.0, 0));
	model_matrix2 = glm::scale(model_matrix2, glm::vec3(size, size, size));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model_matrix2));
	ball.Draw(shader);
	;
	model_matrix3 = glm::translate(model_matrix, glm::vec3(0.15, 0.0, -0.36));
	model_matrix3 = glm::scale(model_matrix3, glm::vec3(size, size, size));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model_matrix3));
	ball.Draw(shader);

	model_matrix4 = glm::translate(model_matrix, glm::vec3(-0.15, 0.0, 0.36));
	model_matrix4 = glm::scale(model_matrix4, glm::vec3(size, size, size));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model_matrix4));
	ball.Draw(shader);

	model_matrix5 = glm::translate(model_matrix, glm::vec3(-0.15, 0.0, 0));
	model_matrix5 = glm::scale(model_matrix5, glm::vec3(size, size, size));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model_matrix5));
	ball.Draw(shader);

	model_matrix6 = glm::translate(model_matrix, glm::vec3(-0.15, 0.0, -0.36));
	model_matrix6 = glm::scale(model_matrix6, glm::vec3(size, size, size));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model_matrix6));
	ball.Draw(shader);
}

void Terrain::translate_terrain_position(float x, float y, float z) {
	terrain_position = glm::vec3(x, y, z);
}

float Terrain::getElasticity()
{
	return Elasticity[type];
}

GLint MarineSnow::Create_Texture_Marine_Snow() {

	int i, j;
	for (i = 0; i < 128; i++) {
		for (j = 0; j < 128; j++) {
			if (pn.noise(0.5 * i, 0.5 * j, 1.0) > 0.8) {
				marine_snow[i][j][0] = 255;
				marine_snow[i][j][1] = 255;
				marine_snow[i][j][2] = 255;
				marine_snow[i][j][3] = 255;
			}
			else {
				marine_snow[i][j][0] = 0;
				marine_snow[i][j][1] = 0;
				marine_snow[i][j][2] = 0;
				marine_snow[i][j][3] = 0;
			}
		}
	}

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 128, 128, 0, GL_RGBA, GL_UNSIGNED_BYTE, marine_snow);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	return textureID;
}

void MarineSnow::init_marine_snow() {
	int i;
	float rx, ry;
	float vertices[30];


	for (i = 0; i < SNOW_NUM; i++) {

		// position
		this->snow[i].position.x = (rand() % 50 - 25)*1.0;
		this->snow[i].position.y = 0.0;
		this->snow[i].position.z = (rand() % 50 - 25)*1.0;
		this->snow[i].position = glm::normalize(this->snow[i].position);
		this->snow[i].position = this->snow[i].position * ((float)((rand() % 20) / 10.0 + 2.0));
		this->snow[i].position.y += (rand() % 10 - 2)*1.0;

		// texture coordinate
		rx = (rand() % 51) / 10.0;
		ry = (rand() % 51) / 10.0;
		memcpy(vertices, faceVertices, sizeof(faceVertices));
		vertices[3] = rx;
		vertices[4] = ry;
		vertices[8] = rx + 0.5;
		vertices[9] = ry;
		vertices[13] = rx + 0.5;
		vertices[14] = ry + 0.5;
		vertices[18] = rx + 0.5;
		vertices[19] = ry + 0.5;
		vertices[23] = rx;
		vertices[24] = ry + 0.5;
		vertices[28] = rx;
		vertices[29] = ry;

		glGenVertexArrays(1, &(this->snow[i].VAO));
		glBindVertexArray(this->snow[i].VAO);

		GLuint VBO;

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// Position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		// texel
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);

	}
	glBindVertexArray(0);
}

void MarineSnow::snow_drop(glm::vec3 eye_position) {
	float rx, ry;
	for (int i = 0; i < SNOW_NUM; i++) {
		this->snow[i].position.y -= this->velocity;
		if (this->snow[i].position.y - eye_position.y < -7.0 ||
			(this->snow[i].position.x - eye_position.x)*(this->snow[i].position.x - eye_position.x) +
			(this->snow[i].position.y - eye_position.y)*(this->snow[i].position.y - eye_position.y) +
			(this->snow[i].position.z - eye_position.z)*(this->snow[i].position.z - eye_position.z) > 7.0 * 7.0) {
			this->snow[i].position.x = (rand() % 50 - 25)*1.0;
			this->snow[i].position.y = 0.0;
			this->snow[i].position.z = (rand() % 50 - 25)*1.0;
			this->snow[i].position = glm::normalize(this->snow[i].position);
			this->snow[i].position = this->snow[i].position * ((float)((rand() % 20)/5.0 + 1)) + eye_position;
			this->snow[i].position.y += (rand() % 10 - 2)*1.0;
		}
	}
}

void MarineSnow::render_marine_snow(glm::vec3 eye_position, Shader shader) {
	shader.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->marineSnowTextureID);
	for (int i = 0; i < SNOW_NUM; i++) {

		// billboard
		glm::vec3 u(eye_position.x - this->snow[i].position.x, 0.0f, eye_position.z - this->snow[i].position.z);
		u = glm::normalize(u);
		glm::vec3 v(0.0f, 1.0f, 0.0f);
		glm::vec3 w = glm::cross(v, u);

		glm::mat4 rotate(0.0);
		rotate[0][0] = u[0];
		rotate[1][0] = u[1];
		rotate[2][0] = u[2];
		rotate[3][0] = 0.0f;

		rotate[0][1] = v[0];
		rotate[1][1] = v[1];
		rotate[2][1] = v[2];
		rotate[3][1] = 0.0f;

		rotate[0][2] = w[0];
		rotate[1][2] = w[1];
		rotate[2][2] = w[2];
		rotate[3][2] = 0.0f;

		rotate[0][3] = 0.0f;
		rotate[1][3] = 0.0f;
		rotate[2][3] = 0.0f;
		rotate[3][3] = 1.0f;

		glm::mat4 model_matrix(1.0);
		model_matrix = glm::translate(model_matrix, this->snow[i].position);
		model_matrix = model_matrix * rotate;
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model_matrix));
		glBindVertexArray(this->snow[i].VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	/*glm::mat4 model_matrix(1.0);
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model_matrix));
	glBindVertexArray(this->marineSnowVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);*/
}
