/*
 * ROV_001
 * Copyright (C) 2019 ROV Simulator
 * National Taiwan Ocean University
 * Department of Computer Science and Engineering
 * Computer Graphics Lab Chieh-Shih Chou, Shyh-Kuang Ueng
 * Copyright (C) 2019
 */
#include "InfoOGLWidget.hpp"

InfoOGLWidget::InfoOGLWidget(QWidget *parent)
	: QOpenGLWidget(parent) , value_queue(3000)
{
	setupOpenGL();

	this->target_value = 0.0;
	this->Y_width = 20.0;

	refreshTimer = new QTimer(this);
	connect(refreshTimer, SIGNAL(timeout()), this, SLOT(update()));
}

InfoOGLWidget::~InfoOGLWidget()
{
	delete refreshTimer;
}

void InfoOGLWidget::initializeGL(void)
{
	makeCurrent();

	//  initialize glew
	if (glewInit() != GLEW_OK) {
		exit(EXIT_FAILURE);
	}

#ifndef _DEBUG
	printf("%s\n", glGetString(GL_VERSION));
	printf("Supported GLSL version is %s.\n", (char *)glGetString(GL_SHADING_LANGUAGE_VERSION));
#endif // !_DEBUG

	InfoShader = new Shader("./Shaders/Info.vs", "./Shaders/Info.fs");
	InfoShader->use();

	initialCoordinatePlane();

	initialCurveBuffer();

	refreshTimer->start(time_interval * 1000);
}

void InfoOGLWidget::paintGL(void)
{
	makeCurrent();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glViewport(0.0, 0.0, this->width(), this->height());

	InfoShader->use();
	InfoShader->setVec2("center_point", center_point);
	InfoShader->setFloat("X_width", X_width);

	paintCoordinatePlane();
	paintTargetLine();

	if(genCurveVertices())
		paintCurve();
	
}

void InfoOGLWidget::resizeGL(int width, int height)
{
	makeCurrent();

	this->widget_width = width;
	this->widget_height = height;
}

void InfoOGLWidget::mousePressEvent(QMouseEvent * e)
{
	makeCurrent();

}

void InfoOGLWidget::mouseMoveEvent(QMouseEvent * e)
{
	makeCurrent();

}

void InfoOGLWidget::mouseReleaseEvent(QMouseEvent * e)
{
	makeCurrent();

}

void InfoOGLWidget::wheelEvent(QWheelEvent * e)
{
	makeCurrent();
	if (this->Y_width + e->angleDelta().y() / 120.0 > 0) {
		this->Y_width += e->angleDelta().y() / 120.0;
	}
	else if(this->Y_width + e->angleDelta().y() / 1200.0 > 0){
		this->Y_width += e->angleDelta().y() / 1200.0;
	}
	else if (this->Y_width + e->angleDelta().y() / 12000.0 > 0) {
		this->Y_width += e->angleDelta().y() / 12000.0;
	}
}

void InfoOGLWidget::setupOpenGL(void)
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

void InfoOGLWidget::initialCoordinatePlane(void)
{
	makeCurrent();
	X_Axis_Line_Vertices[0] = glm::vec2(-1.0, 0.0);
	X_Axis_Line_Vertices[1] = glm::vec2(50.0, 0.0);

	Y_Axis_Line_Vertices[0] = glm::vec2(0.0, 500.0);
	Y_Axis_Line_Vertices[1] = glm::vec2(0.0, -500.0);

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
}

void InfoOGLWidget::initialCurveBuffer(void)
{
	makeCurrent();
	glGenVertexArrays(1, &curve_VAO);
	glBindVertexArray(curve_VAO);
	glEnableVertexAttribArray(0);


	glGenBuffers(1, &Curve_Vertices_Buffer);
}

void InfoOGLWidget::paintCoordinatePlane(void)
{
	makeCurrent();
	// Axis-X
	InfoShader->setVec3("LineColor", glm::vec3(1.0, 1.0, 1.0));
	glBindVertexArray(axisline_VAO[0]);
	glLineWidth(2.0);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);
	// Axis-Y
	InfoShader->setVec3("LineColor", glm::vec3(1.0, 1.0, 1.0));
	glBindVertexArray(axisline_VAO[1]);
	glLineWidth(2.0);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);
}

void InfoOGLWidget::paintTargetLine(void)
{
	makeCurrent();
	if (this->target_value == 0.0)
		return;

	// Target Line
	InfoShader->setVec3("LineColor", glm::vec3(1.0, 0.0, 0.0));
	glBindVertexArray(targetline_VAO);
	glLineWidth(2.0);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);
}

bool InfoOGLWidget::genCurveVertices()
{
	makeCurrent();
	InfoShader->setFloat("Y_width", Y_width);
	if (this->value_queue.getSize() <= 0)
		return false;

	glm::vec2* vertices = new glm::vec2[value_queue.getSize()];

	for (int i = 0; i < value_queue.getSize(); i++) {
		vertices[i] = glm::vec2(fmod(i * time_interval, 50), value_queue[i]);
	}

	glBindBuffer(GL_ARRAY_BUFFER, Curve_Vertices_Buffer);
	glBufferData(GL_ARRAY_BUFFER, value_queue.getSize() * 2 * sizeof(float), vertices, GL_DYNAMIC_DRAW);

	// bind axis-x VAO
	glBindVertexArray(curve_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, Curve_Vertices_Buffer);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	delete[] vertices;

	return true;
}

void InfoOGLWidget::paintCurve(void)
{
	makeCurrent();
	if (this->value_queue.getSize() <= 0)
		return;
	InfoShader->setVec3("LineColor", glm::vec3(0.0, 1.0, 0.0));
	glBindVertexArray(curve_VAO);
	glLineWidth(2.0);
	glDrawArrays(GL_LINE_STRIP, 0, value_queue.getSize());
	glBindVertexArray(0);
}

void InfoOGLWidget::setTimeInterval(float value)
{
	makeCurrent();
	this->time_interval = value;
}

void InfoOGLWidget::record(float recent_status_value)
{
	makeCurrent();
	this->value_queue.push(recent_status_value);
}

void InfoOGLWidget::setTarget(float value)
{
	makeCurrent();
	this->target_value = value;
	center_point.y = this->target_value;

	target_Line_Vertices[0] = glm::vec2(X_Axis_Line_Vertices[0].x, this->target_value);
	target_Line_Vertices[1] = glm::vec2(X_Axis_Line_Vertices[1].x, this->target_value);


	GLuint target_Line_Vertice_Buffer;
	glGenBuffers(1, &target_Line_Vertice_Buffer);
	glBindBuffer(GL_ARRAY_BUFFER, target_Line_Vertice_Buffer);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(float), target_Line_Vertices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &targetline_VAO);
	// bind axis-x VAO
	glBindVertexArray(targetline_VAO);

	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, target_Line_Vertice_Buffer);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
}

void InfoOGLWidget::setY_width(float value)
{
	makeCurrent();
	this->Y_width = value;
}
