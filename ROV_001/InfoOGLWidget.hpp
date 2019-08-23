/*
 * ROV_001
 * Copyright (C) 2019 ROV Simulator
 * National Taiwan Ocean University
 * Department of Computer Science and Engineering
 * Lab 503 Chieh-Shih Chou, Shyh-Kuang Ueng
 * Copyright (C) 2019
 */
#pragma once

#include "GL/glew.h"
#include "Shader.hpp"
#include <QOpenGLWidget>
#include <QWheelEvent>
#include <QTimer>
#include "MySequentialQueueArray.hpp"
#include <math.h>
#include <iostream>

class InfoOGLWidget : public QOpenGLWidget
{
	Q_OBJECT

// method
public:
	InfoOGLWidget(QWidget *parent = Q_NULLPTR);
	~InfoOGLWidget();

protected:
	// opengl rendering method
	void initializeGL(void);
	void paintGL(void);
	void resizeGL(int width, int height);
	void mousePressEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void wheelEvent(QWheelEvent *e);

private:
	// rendering method
	void setupOpenGL(void);
	void initialCoordinatePlane(void);
	void initialCurveBuffer(void);
	void paintCoordinatePlane(void);
	void paintTargetLine(void);
	bool genCurveVertices(void);
	void paintCurve(void);

public:
	void setTimeInterval(float);	// second
	void record(float);
	void setTarget(float);
	void setY_width(float);

// member
private:
	QTimer * refreshTimer;

	// ui parameter
	float widget_width, widget_height;
	GLuint axisline_VAO[2];		// 0 is axis-x, 1 is axis-y
	GLuint targetline_VAO;
	GLuint curve_VAO;
	GLuint Curve_Vertices_Buffer;
	Shader* InfoShader;

	glm::vec2 X_Axis_Line_Vertices[2];
	glm::vec2 Y_Axis_Line_Vertices[2];
	glm::vec2 target_Line_Vertices[2];
	glm::vec2 center_point;
	float X_width;
	float Y_width;
	float time_interval;	// second


	// PID parameter
	float target_value;
	MySequentialQueueArray<float> value_queue;




public:



};
