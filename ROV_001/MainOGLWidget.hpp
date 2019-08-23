/*
 * ROV_001
 * Copyright (C) 2019 ROV Simulator
 * National Taiwan Ocean University
 * Department of Computer Science and Engineering
 * Computer Graphics Lab Chieh-Shih Chou, Shyh-Kuang Ueng
 * Copyright (C) 2019
 */
#pragma once
// std include
#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>
#include <windows.h> 
#include <chrono>

// GL include
#define GLEW_STATIC
#include "GL/glew.h"
#include <QOpenGLWidget>
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model.hpp"
#include "Scene.hpp"
#include "MyCubeMap.hpp"

// QT include
#include <QMouseEvent>
#include <QDebug>
#include <QTimer>
#include <QMenu>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include "MyValidator.hpp"
#include "ui_MainOGLWidget.h"
#include "InfoCouplingOGLWidget.hpp"

// Other Include
// ROV object
#include "ROV.hpp"
#include "Oilplatform.hpp"
#include "MyWaterPlane.hpp"


#define MAINCAMERA 0
#define SUBCAMERA_1 1
#define SUBCAMERA_2 2
#define SUBCAMERA_3 3
#define SUBCAMERA_4 4

#define SCREENAMOUNT 5
#define MAINSCREEN 0
#define SUBSCREEN_1 1
#define SUBSCREEN_2 2
#define SUBSCREEN_3 3
#define SUBSCREEN_4 4

#define HandleBarPushMaxRange 16384
#define HandleBarYawMaxRange 256
#define PuttMaxRange 255

#define HeaveMaxSpeed 1.0
#define SurgeMaxSpeed 1.0
#define SwayMaxSpeed 1.0
#define YawMaxRadianSpeed 0.31415926

enum Joystick { Joystick1, Joystick2, JoystickAmount };

class MainOGLWidget : public QOpenGLWidget
{
	Q_OBJECT
public:
    /* method */
	MainOGLWidget(QWidget *parent = Q_NULLPTR);
	~MainOGLWidget();

protected:
	// opengl rendering method
	void initializeGL(void);
	void paintGL(void);
	void resizeGL(int width, int height);
	void mousePressEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void wheelEvent(QWheelEvent *e);
	void keyPressEvent(QKeyEvent *e);
	void keyReleaseEvent(QKeyEvent *e);
	

private:
	// rendering method
	/*initial*/
	void setupOpenGL(void);
	void initialShader(void);
	void initialCameraMatrixUBO(void);
	void initialEnvironmentLight(void);
	void initialScene(void);
	void initialMarineSnow(void);
	void initialSkyBox(void);
	void initialUIParameter(void);
	void initialCompassTexture(void);
	void initialRoomTexture(void);
	void initialScreenViewportParameter(void);
	void initialCameraParameter(void);
	void initialCameraMenu(void);
	void initialPathLine(void);

	/*calculate*/
	void calculateScreenViewportParameter(void);
	void calculateCameraPameter(void);

	/*draw*/
	void paintScreen(int screenNumber);
	void paintCompass(void);
	void paintRoom(void);
	void paintROV(void);
	void paintScene(void);
	void paintWaterPlane();
	void paintMarineSnow(glm::vec3 view_position);
	void paintROVPath();

	//sound method
	void initialSound(void);
	void startBackgroundSound(void);
	void stopBackgroundSound(void);

public:
	void reSetEnvironment(void);
	void setSceneType(int);
	void setMarineSnowUse(bool);
	void setEnvironmentLightParameter(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 direction, glm::vec3 attenuation);

private:
signals:
	void warning(bool);

	// SLOTS 
private slots:
	/*timer*/
	void PhysicsEngine_Func(void);
	void setInformation(void);
	void recordPIDInformation(void);

	// Function Slots
	// Linear Motion
	void changeHeaveValue(int);
	void changeSurgeValue(int);
	void changeSwayValue(int);
	// Angular Motion
	void changeYawValue(int);
	void changeRollValue(int);
	void changePitchValue(int);
	// Change Control Mode
	void change2JoystickMode(bool);
	void change2ManuMode(bool);
	// Switch Robot Arm
	void switch_controlling_robotarm(bool);
	// Show Bounding Volume
	void switch_displaying_boundingvolume(bool);
	// Show Path Line
	void switch_recording_pathline(bool);
	// Change mode of propeller LCD 
    void on_PowerButton_clicked(bool checked);
    void on_RPMButton_clicked(bool checked);
    void on_ForceButton_clicked(bool checked);
	// Camera change
	void changeCamera(QAction*);
    // PID
    void on_setPositionTarget_PB_clicked(void);
	void on_setOrientationTarget_PB_clicked(void);
	// joystick
	void processMainROVControlJoystick(void);
	void processRobotArmControlJoystick(void);
	void decodingJoyButton(int num_of_joystick, vector<int>* press_button_array, vector<int>* release_button_array);
	void processPressButton(int buttonID, int joy_number);
	void processReleaseButton(int buttonID, int joy_number);
	// Sound
	void playWarning(bool);
	// Water plane
	void updateWaterTime(void);
	// Path line
	void recordPathPosition(void);
	// calculate fps
	void myUpdate();


private:
    /* member */
	// ui
	Ui::MainOGLWidget ui;
	// timer
	QTimer* refreshTimer;
	QTimer* physicsEngineTimer;
	QTimer* refreshInformationTimer;
	QTimer* recordPIDWidgetTimer;
	QTimer* mainROVControlJoystickTimer;
	QTimer* robotArmControlJoystickTimer;
	QTimer* bubble_sound_timer[2];
	QTimer* propeller_sound_timer[2];
	QTimer* warning_sound_timer[2];
	QTimer* waterplaneTimer;
	QTimer* pathlineTimer;
	// menu
	QMenu* function_menu;
	QMenu* main_screen_camera_menu;
	QMenu* sub_screen_camera_menu;

	// sound
	QMediaPlayer* bubble_player[2];
	QMediaPlayer* propeller_player[2];
	QMediaPlayer* warning_player[2];

	//
	InfoCouplingOGLWidget* infocouplingWidget;

	//////////////////////////////////////////////////////
	// initial value:
	// dividerPosition:	
	//					divider position x
	//					main screen width
	//					
	// subScreenWidth:	
	//					sub screen width
	// maxSubScreenWidth:
	//					max width value of subscreen
	//					value = Monitor - 210(the other total width) / 3.0
	//					so maxSubScreenWidth equal to One-third of total screen width,include main screen and sub screen.
	//////////////////////////////////////////////////////
	float	browser_height;
	float	dividerPosition;
	float	subScreenWidth;
	float	maxSubScreenWidth;
	float	screenViewport[SCREENAMOUNT][4];	//[0]: posX, [1]: posY, [2]:width, [3]:height  
	int		screenCameraID[SCREENAMOUNT];

	glm::vec4 foggyColor;

	Camera_t* camera;
	int total_camera_amount;
	glm::vec2 change_camera_pos;

	Shader* basicShader;
	Shader* compassShader;
	Shader* skyboxShader;
	Shader* marinesnowShader;
	Shader* waterplaneShader;
	Shader* pathlineShader;
	Shader* boundingboxShader;
	Shader* ballShader;

	float frameTime;						// second
	float PhisicsEngineTimeInterval;		// second
	float informationRefreshTime;			// second
	float PIDInformationRecordTime;			// second
		
	// light object
	Light *environment_light = nullptr;
	glm::vec3 environment_light_ambient;
	glm::vec3 environment_light_diffuse;
	glm::vec3 environment_light_specular;
	glm::vec3 environment_light_direction;
	glm::vec3 environment_light_attenuation;

	// texture object
	GLuint texture_compass[2];
	GLuint texture_ROV[3];
	GLuint texture_checkerboard;
	// Cubemap
	bool loadskybox = false;
	GLuint cubemap;
	// Skybox VAO
	GLuint skyBoxVAO;
	// Camera UBO
	GLuint cameraMatrixUBO;

	// mouse para,eter
	bool show_cursor;
	glm::vec2 old_mouse_pos;

	// PID Parameter
	glm::vec3 global_target_position;
	glm::vec3 target_orientation;
	glm::vec3 target_velocity;
	glm::vec3 target_angular_velocity;

	// Mode
	bool mode_joystick;
	bool mode_manu;
	// joystick
	joyinfoex_tag* joystick_info[JoystickAmount];
	int old_button_amount[JoystickAmount];
	int old_button_number[JoystickAmount];
	bool auto_depth = false;
	bool auto_heading = false;
	bool stable_attitude = false;
	// menu_motion_power
	float menu_motionPower[NumMotion] = { 0 };

	// render object and parameter
	ROV* rov;
	Model* ball;
	Oilplatform* oilplatform;
	Terrain* seabed;
	MyWaterPlane* waterplane;
	MySequentialQueueArray<glm::vec3> position_list;
	GLuint pathLineVAO, pathLineVBO;
	int scene_type = 0;
	bool terrain_scene_loaded = false;
	MarineSnow* marinesnow;
	bool marinesnow_use = false;
	float water_time = 0.0;
	bool showboundingbox = false;
	bool showphysicscenter = false;
	bool showpathline = false;


	CollisionState collision_state = CollisionState::Safe;
	CollisionState old_collision_state = CollisionState::Safe;
};
