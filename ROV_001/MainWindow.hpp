/*
 * ROV_001
 * Copyright (C) 2019 ROV Simulator
 * National Taiwan Ocean University
 * Department of Computer Science and Engineering
 * Lab 503 Chieh-Shih Chou, Shyh-Kuang Ueng
 * Copyright (C) 2019
 */
#pragma once
#include "SceneChooseWidget.hpp"
#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow(QWidget *parent = Q_NULLPTR);
	~MainWindow();
private:
	void shootParameter(void);

public:
signals:
	void sent_robot_arm_station(bool on_off);
	void sent_bounding_volume_station(bool on_off);
	void sent_path_line_station(bool on_off);
private slots:
    void on_mode_manu_clicked(bool checked);
    void on_mode_joystick_clicked(bool checked);
	void on_actionEnvironment_Setting_triggered(bool checked);
	void on_robot_arm_switch_button_clicked(bool checked);
    void on_ShowBoundingVolume_button_clicked();

    void on_RecordPath_button_clicked();

private:
	Ui::MainWindowClass ui;
	SceneChooseWidget scenechoosewidget;


};
