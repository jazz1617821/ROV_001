/*
 * ROV_001
 * Copyright (C) 2019 ROV Simulator
 * National Taiwan Ocean University
 * Department of Computer Science and Engineering
 * Computer Graphics Lab Chieh-Shih Chou, Shyh-Kuang Ueng
 * Copyright (C) 2019
 */

#include "InfoCouplingOGLWidget.hpp"

InfoCouplingOGLWidget::InfoCouplingOGLWidget(QWidget *parent) 
	: QOpenGLWidget(parent), data{ 600, 600, 600, 600, 600, 600}
{
	ui.setupUi(this);

	this->setWindowFlags(Qt::Window);

	setupOpenGL();

	X_Alied_Scale_Vertices.clear();
	Y_Alied_Scale_Vertices.clear();

	Y_width = 10.0;

	refreshTimer = new QTimer(this);
	connect(refreshTimer, SIGNAL(timeout()), this, SLOT(update()));
}

InfoCouplingOGLWidget::~InfoCouplingOGLWidget()
{
	delete refreshTimer;
}

void InfoCouplingOGLWidget::initializeGL(void)
{
	makeCurrent();

	//  initialize glew
	if (glewInit() != GLEW_OK) {
		exit(EXIT_FAILURE);
	}

#ifdef _DEBUG
	printf("%s\n", glGetString(GL_VERSION));
	printf("Supported GLSL version is %s.\n", (char *)glGetString(GL_SHADING_LANGUAGE_VERSION));
#endif // !_DEBUG

	InfoShader = new Shader("./Shaders/Info.vs", "./Shaders/Info.fs");
	InfoShader->use();

	initialCoordinatePlane();
	//initialCurveBuffer();

	refreshTimer->start(time_interval * 1000);
}

void InfoCouplingOGLWidget::paintGL(void)
{
	makeCurrent();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0, 1.0, 1.0, 1.0);

	glViewport(0.0, 0.0, this->width(), this->height());

	InfoShader->use();
	InfoShader->setVec2("center_point", center_point);
	InfoShader->setFloat("X_width", X_width);
	InfoShader->setFloat("Y_width", Y_width);

	paintCoordinatePlane();
	
	paintCurve();
}

void InfoCouplingOGLWidget::resizeGL(int width, int height)
{
	makeCurrent();

	this->widget_width = width;
	this->widget_height = height;
}

void InfoCouplingOGLWidget::mousePressEvent(QMouseEvent * e)
{
	makeCurrent();
}

void InfoCouplingOGLWidget::mouseMoveEvent(QMouseEvent * e)
{
	makeCurrent();
}

void InfoCouplingOGLWidget::mouseReleaseEvent(QMouseEvent * e)
{
	makeCurrent();
}

void InfoCouplingOGLWidget::wheelEvent(QWheelEvent * e)
{
	makeCurrent();
}

void InfoCouplingOGLWidget::setupOpenGL(void)
{
	makeCurrent();

	QSurfaceFormat format = QSurfaceFormat::defaultFormat();

	format.setDepthBufferSize(64);
	format.setStencilBufferSize(8);
	format.setVersion(4, 3);
	format.setProfile(QSurfaceFormat::CoreProfile);
	QSurfaceFormat::setDefaultFormat(format);

	setFocusPolicy(Qt::StrongFocus);
}

void InfoCouplingOGLWidget::initialCoordinatePlane(void)
{
	makeCurrent();
	// initial Axis
	X_Axis_Line_Vertices[0] = glm::vec2(-0.2, 0.0);
	X_Axis_Line_Vertices[1] = glm::vec2(TotalRecordingTime, 0.0);

	Y_Axis_Line_Vertices[0] = glm::vec2(0.0, Y_width);
	Y_Axis_Line_Vertices[1] = glm::vec2(0.0, -Y_width);

	center_point = glm::vec2((X_Axis_Line_Vertices[1].x + X_Axis_Line_Vertices[0].x) / 2.0, 0.0);
	X_width = (X_Axis_Line_Vertices[1].x - (X_Axis_Line_Vertices[0].x)) / 2.0;

	// 
	GLuint X_Axis_Vertice_Buffer;
	glGenBuffers(1, &X_Axis_Vertice_Buffer);
	glBindBuffer(GL_ARRAY_BUFFER, X_Axis_Vertice_Buffer);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(float), X_Axis_Line_Vertices, GL_STATIC_DRAW);

	GLuint Y_Axis_Vertice_Buffer;
	glGenBuffers(1, &Y_Axis_Vertice_Buffer);
	glBindBuffer(GL_ARRAY_BUFFER, Y_Axis_Vertice_Buffer);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(float), Y_Axis_Line_Vertices, GL_STATIC_DRAW);


	glGenVertexArrays(2, axisline_VAO);
	// bind axis-x VAO
	glBindVertexArray(axisline_VAO[0]);

	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, X_Axis_Vertice_Buffer);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	// bind axis-y VAO
	glBindVertexArray(axisline_VAO[1]);

	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, Y_Axis_Vertice_Buffer);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindVertexArray(0);

	// initial scale
	// 橫向刻度 
	int amount;
	float length = Y_Axis_Line_Vertices[0].y - Y_Axis_Line_Vertices[1].y;
	float scale_size = 1.0;
	amount = length / scale_size;
	for (int n = 0; n < amount; n++) {
		glm::vec2 v0 = glm::vec2(X_Axis_Line_Vertices[0].x, Y_Axis_Line_Vertices[1].y + n * scale_size);
		glm::vec2 v1 = glm::vec2(X_Axis_Line_Vertices[1].x, Y_Axis_Line_Vertices[1].y + n * scale_size);
		X_Alied_Scale_Vertices.push_back(v0);
		X_Alied_Scale_Vertices.push_back(v1);
	}
	// 縱向刻度 
	amount;
	length = X_Axis_Line_Vertices[1].x - 0.0;
	scale_size = 1.0;
	amount = length / scale_size;
	for (int n = 0; n < amount; n++) {
		glm::vec2 v0 = glm::vec2(0.0 + n * scale_size, Y_Axis_Line_Vertices[0].y);
		glm::vec2 v1 = glm::vec2(0.0 + n * scale_size, Y_Axis_Line_Vertices[1].y);
		Y_Alied_Scale_Vertices.push_back(v0);
		Y_Alied_Scale_Vertices.push_back(v1);
	}
	GLuint X_Scale_Vertices_Buffer;
	glGenBuffers(1, &X_Scale_Vertices_Buffer);
	glBindBuffer(GL_ARRAY_BUFFER, X_Scale_Vertices_Buffer);
	glBufferData(GL_ARRAY_BUFFER, X_Alied_Scale_Vertices.size() * sizeof(glm::vec2), &X_Alied_Scale_Vertices[0], GL_STATIC_DRAW);

	GLuint Y_Scale_Vertices_Buffer;
	glGenBuffers(1, &Y_Scale_Vertices_Buffer);
	glBindBuffer(GL_ARRAY_BUFFER, Y_Scale_Vertices_Buffer);
	glBufferData(GL_ARRAY_BUFFER, Y_Alied_Scale_Vertices.size() * sizeof(glm::vec2), &Y_Alied_Scale_Vertices[0], GL_STATIC_DRAW);


	glGenVertexArrays(2, scaleline_VAO);
	// bind axis-x VAO
	glBindVertexArray(scaleline_VAO[0]);

	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, X_Scale_Vertices_Buffer);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), NULL);

	// bind axis-y VAO
	glBindVertexArray(scaleline_VAO[1]);

	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, Y_Scale_Vertices_Buffer);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), NULL);
	glBindVertexArray(0);
}

void InfoCouplingOGLWidget::initialCurveBuffer(void)
{
	makeCurrent();
	glGenVertexArrays(6, curve_VAO);
	glGenBuffers(6, Curve_Vertices_Buffer);
}

void InfoCouplingOGLWidget::paintCoordinatePlane(void)
{
	makeCurrent();
	// Axis-X
	InfoShader->setVec3("LineColor", glm::vec3(0.0, 0.0, 0.0));
	glBindVertexArray(axisline_VAO[0]);
	glLineWidth(4.0);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);
	// Axis-Y
	InfoShader->setVec3("LineColor", glm::vec3(0.0, 0.0, 0.0));
	glBindVertexArray(axisline_VAO[1]);
	glLineWidth(4.0);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);

	// Paint Scale
	// 橫向刻度
	InfoShader->setVec3("LineColor", glm::vec3(0.0, 0.0, 0.0));
	glBindVertexArray(scaleline_VAO[0]);
	glLineWidth(1.0);
	glDrawArrays(GL_LINES, 0, X_Alied_Scale_Vertices.size());
	glBindVertexArray(0);
	// 縱向刻度
	InfoShader->setVec3("LineColor", glm::vec3(0.0, 0.0, 0.0));
	glBindVertexArray(scaleline_VAO[1]);
	glLineWidth(1.0);
	glDrawArrays(GL_LINES, 0, Y_Alied_Scale_Vertices.size());
	glBindVertexArray(0);
}

void InfoCouplingOGLWidget::paintCurve(void)
{
	makeCurrent();
	glm::vec3 color[6] = {
		glm::vec3(1.0,0.0,0.0),
		glm::vec3(0.0,1.0,0.0),
		glm::vec3(0.0,0.0,1.0),
		glm::vec3(0.0,1.0,1.0),
		glm::vec3(1.0,0.0,1.0),
		glm::vec3(1.0,1.0,0.0)
	};
	for (int i = 0; i < 6; i++) {
		if (this->data[i].getSize() <= 0)
			continue;

		vector<glm::vec2> vertices;

		for (int j = 0; j < data[i].getSize(); j++) {
			vertices.push_back(glm::vec2(fmod(j * time_interval, TotalRecordingTime), data[i][j]));
		}

		glBindVertexArray(curve_VAO[i]);
		glBindBuffer(GL_ARRAY_BUFFER, Curve_Vertices_Buffer[i]);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2), &vertices[0], GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), NULL);


		InfoShader->setVec3("LineColor", color[i]);
		glBindVertexArray(curve_VAO[i]);
		glLineWidth(2.0);
		glDrawArrays(GL_LINE_STRIP, 0, data[i].getSize());
		glBindVertexArray(0);
	}
}

void InfoCouplingOGLWidget::setTimeInterval(float value)
{
	this->time_interval = value;
}

void InfoCouplingOGLWidget::record(glm::vec3 position, glm::vec3 orientation)
{
	makeCurrent();
	this->data[0].push(position.x);
	this->data[1].push(position.y);
	this->data[2].push(position.z);
	this->data[3].push(orientation.x);
	this->data[4].push(orientation.y);
	this->data[5].push(orientation.z);
}
