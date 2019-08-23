/*
 * ROV_001
 * Copyright (C) 2019 ROV Simulator
 * National Taiwan Ocean University
 * Department of Computer Science and Engineering
 * Computer Graphics Lab Chieh-Shih Chou, Shyh-Kuang Ueng
 * Copyright (C) 2019
 */
#include "MainWindow.hpp"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	scenechoosewidget.show();
	QEventLoop loop;
	connect(&scenechoosewidget, SIGNAL(closed()), &loop, SLOT(quit()));
	loop.exec();

	shootParameter();

	// Function connect
	// Linear Motion
	connect(this->ui.HeaveValue_Slider, SIGNAL(valueChanged(int)), ui.OGLWidge_main, SLOT(changeHeaveValue(int)));
	connect(this->ui.SurgeValue_Slider, SIGNAL(valueChanged(int)), ui.OGLWidge_main, SLOT(changeSurgeValue(int)));
	connect(this->ui.SwayValue_Slider, SIGNAL(valueChanged(int)), ui.OGLWidge_main, SLOT(changeSwayValue(int)));
	// Angular Motion
	connect(this->ui.YawValue_Slider, SIGNAL(valueChanged(int)), ui.OGLWidge_main, SLOT(changeYawValue(int)));
	connect(this->ui.RollValue_Slider, SIGNAL(valueChanged(int)), ui.OGLWidge_main, SLOT(changeRollValue(int)));
	connect(this->ui.PitchValue_Slider, SIGNAL(valueChanged(int)), ui.OGLWidge_main, SLOT(changePitchValue(int)));

	// Mode change
	connect(this->ui.mode_joystick, SIGNAL(clicked(bool)), ui.OGLWidge_main, SLOT(change2JoystickMode(bool)));
	connect(this->ui.mode_manu, SIGNAL(clicked(bool)), ui.OGLWidge_main, SLOT(change2ManuMode(bool)));
	// Robot Arm Switch
	connect(this, SIGNAL(sent_robot_arm_station(bool)), ui.OGLWidge_main, SLOT(switch_controlling_robotarm(bool)));
	// Bounding Volume Switch
	connect(this, SIGNAL(sent_bounding_volume_station(bool)), ui.OGLWidge_main, SLOT(switch_displaying_boundingvolume(bool)));
	// Bounding Volume Switch
	connect(this, SIGNAL(sent_path_line_station(bool)), ui.OGLWidge_main, SLOT(switch_recording_pathline(bool)));
}

MainWindow::~MainWindow()
{

}

void MainWindow::shootParameter(void)
{
	this->ui.OGLWidge_main->setSceneType(scenechoosewidget.scene_type);
	this->ui.OGLWidge_main->setMarineSnowUse(scenechoosewidget.ui.MarineSnow_CB->isChecked());
	this->ui.OGLWidge_main->setEnvironmentLightParameter(
		glm::vec3(scenechoosewidget.ui.Ambient_R->value(), scenechoosewidget.ui.Ambient_G->value(), scenechoosewidget.ui.Ambient_B->value()),
		glm::vec3(scenechoosewidget.ui.Diffuse_R->value(), scenechoosewidget.ui.Diffuse_G->value(), scenechoosewidget.ui.Diffuse_B->value()),
		glm::vec3(scenechoosewidget.ui.Specular_R->value(), scenechoosewidget.ui.Specular_G->value(), scenechoosewidget.ui.Specular_B->value()),
		glm::vec3(scenechoosewidget.ui.Direction_X->value(), scenechoosewidget.ui.Direction_Y->value(), scenechoosewidget.ui.Direction_Z->value()),
		glm::vec3(scenechoosewidget.ui.Attenuation_C->value(), scenechoosewidget.ui.Attenuation_L->value(), scenechoosewidget.ui.Attenuation_Q->value())
	);
}

void MainWindow::on_mode_manu_clicked(bool checked)
{
    ui.mode_joystick->setChecked(!checked);
}

void MainWindow::on_mode_joystick_clicked(bool checked)
{
    ui.mode_manu->setChecked(!checked);
}

void MainWindow::on_actionEnvironment_Setting_triggered(bool checked)
{
	scenechoosewidget.show();
	QEventLoop loop;
	connect(&scenechoosewidget, SIGNAL(closed()), &loop, SLOT(quit()));
	loop.exec();

	shootParameter();
	this->ui.OGLWidge_main->reSetEnvironment();
}

void MainWindow::on_robot_arm_switch_button_clicked(bool on_off)
{
	emit sent_robot_arm_station(ui.robot_arm_switch_button->isChecked());
}

void MainWindow::on_ShowBoundingVolume_button_clicked()
{
	emit sent_bounding_volume_station(ui.ShowBoundingVolume_button->isChecked());
}

void MainWindow::on_RecordPath_button_clicked()
{
	emit sent_path_line_station(ui.RecordPath_button->isChecked());
}
