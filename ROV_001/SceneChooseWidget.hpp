/*
 * ROV_001
 * Copyright (C) 2019 ROV Simulator
 * National Taiwan Ocean University
 * Department of Computer Science and Engineering
 * Lab 503 Chieh-Shih Chou, Shyh-Kuang Ueng
 * Copyright (C) 2019
 */
#pragma once

#include <QWidget>
#include "glm/glm.hpp"
#include "ui_SceneChooseWidget.h"

class SceneChooseWidget : public QWidget
{
	Q_OBJECT

public:
	SceneChooseWidget(QWidget *parent = Q_NULLPTR);
	~SceneChooseWidget();

signals:
	void closed();

private slots:
	void on_SandButton_clicked();
	void on_RockButton_clicked();
	void on_RoomButton_clicked();
    void on_OK_clicked();

public:
	Ui::SceneChooseWidget ui;
	int scene_type = 0;

};
