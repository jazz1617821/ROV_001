/*
 * ROV_001
 * Copyright (C) 2019 ROV Simulator
 * National Taiwan Ocean University
 * Department of Computer Science and Engineering
 * Computer Graphics Lab Chieh-Shih Chou, Shyh-Kuang Ueng
 * Copyright (C) 2019
 */
#include "SceneChooseWidget.hpp"

SceneChooseWidget::SceneChooseWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	this->setGeometry(0.0,0.0,300.0,300.0);
	this->setWindowFlags(Qt::Window);
	this->setWindowFlags(Qt::WindowTitleHint);

	// set image
	
	this->ui.SandButton->setIcon(QIcon("./Image/Scene/Sand.jpg"));
	this->ui.SandButton->setIconSize(QSize(512, 300));
	this->ui.RockButton->setIcon(QIcon("./Image/Scene/Rock.jpg"));
	this->ui.RockButton->setIconSize(QSize(512, 300));
	this->ui.RoomButton->setIcon(QIcon("./Image/Scene/Room.jpg"));
	this->ui.RoomButton->setIconSize(QSize(512, 300));
}


SceneChooseWidget::~SceneChooseWidget()
{

}

void SceneChooseWidget::on_SandButton_clicked()
{
	this->scene_type = 0;
	this->ui.SandButton->setChecked(true);
	this->ui.RoomButton->setChecked(false);
	this->ui.RockButton->setChecked(false);
}

void SceneChooseWidget::on_RockButton_clicked()
{
	this->scene_type = 1;
	this->ui.RockButton->setChecked(true);
	this->ui.RoomButton->setChecked(false);
	this->ui.SandButton->setChecked(false);
}

void SceneChooseWidget::on_RoomButton_clicked()
{
	this->scene_type = 2;
	this->ui.RoomButton->setChecked(true);
	this->ui.RockButton->setChecked(false);
	this->ui.SandButton->setChecked(false);
}

void SceneChooseWidget::on_OK_clicked()
{
	this->close();
	emit closed();
}

