/*
 * ROV_001
 * Copyright (C) 2019 ROV Simulator
 * National Taiwan Ocean University
 * Department of Computer Science and Engineering
 * Computer Graphics Lab Chieh-Shih Chou, Shyh-Kuang Ueng
 * Copyright (C) 2019
 */

#pragma once

#include "GL/glew.h"
#include "Shader.hpp"
#include <QOpenGLWidget>
#include "ui_InfoCouplingOGLWidget.h"
#include <QWheelEvent>
#include <QTimer>
#include "MySequentialQueueArray.hpp"
#include <math.h>
#include <iostream>
#include <vector>

#define TotalRecordingTime 10		//second

using namespace std;

class InfoCouplingOGLWidget : public QOpenGLWidget
{
	Q_OBJECT

public:
	InfoCouplingOGLWidget(QWidget *parent = Q_NULLPTR);
	~InfoCouplingOGLWidget();

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
	void paintCurve(void);

public:
	void setTimeInterval(float);	// second
	void record(glm::vec3 position, glm::vec3 orientation);

private:
	QTimer * refreshTimer;

	// ui
	Ui::InfoCouplingOGLWidget ui;
	float widget_width, widget_height;
	GLuint axisline_VAO[2];				// 0 is axis-x, 1 is axis-y
	GLuint scaleline_VAO[2];			// 0 橫向刻度 , 1 縱向刻度
	GLuint curve_VAO[6];				// data VAO
	GLuint Curve_Vertices_Buffer[6];	// Store data vertices
	Shader* InfoShader;

	// ui parameter
	float time_interval;	// second
	glm::vec2 center_point;
	float X_width;
	float Y_width;

	// Axis vertice
	glm::vec2 X_Axis_Line_Vertices[2];
	glm::vec2 Y_Axis_Line_Vertices[2];
	// Scale vertice
	vector<glm::vec2> X_Alied_Scale_Vertices;
	vector<glm::vec2> Y_Alied_Scale_Vertices;


	// info data
	MySequentialQueueArray<float> data[6];		// position and orientation
};
