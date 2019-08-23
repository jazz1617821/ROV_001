/*
 * ROV_001
 * Copyright (C) 2019 ROV Simulator
 * National Taiwan Ocean University
 * Department of Computer Science and Engineering
 * Computer Graphics Lab Chieh-Shih Chou, Shyh-Kuang Ueng
 * Copyright (C) 2019
 */

#include "MainOGLWidget.hpp"

/*local*/


/**/
MainOGLWidget::MainOGLWidget(QWidget *parent)
	: QOpenGLWidget(parent) , position_list(3000)
{	
	// ui setting
	ui.setupUi(this);

	setupOpenGL();
	infocouplingWidget = new InfoCouplingOGLWidget(this);

	/* Construct Timer */

	/*
	// fps Timer
	*/
	refreshTimer = new QTimer(this);
	connect(refreshTimer, SIGNAL(timeout()), this, SLOT(myUpdate()));

	/* 
	// Physics engine Timer
	*/
	physicsEngineTimer = new QTimer(this);
	connect(physicsEngineTimer, SIGNAL(timeout()), this, SLOT(PhysicsEngine_Func()));

	/* 
	// Information Timer 
	*/
	refreshInformationTimer = new QTimer(this);
	connect(refreshInformationTimer, SIGNAL(timeout()), this, SLOT(setInformation()));

	/*
	// Record PID Information Timer
	*/
	recordPIDWidgetTimer = new QTimer(this);
	connect(recordPIDWidgetTimer, SIGNAL(timeout()), this, SLOT(recordPIDInformation()));

	/*
	// Joystick Timer
	*/
	mainROVControlJoystickTimer = new QTimer(this);
	connect(mainROVControlJoystickTimer, SIGNAL(timeout()), this, SLOT(processMainROVControlJoystick()));

	robotArmControlJoystickTimer = new QTimer(this);
	connect(robotArmControlJoystickTimer, SIGNAL(timeout()), this, SLOT(processRobotArmControlJoystick()));

	/**/
	waterplaneTimer = new QTimer(this);
	connect(waterplaneTimer, SIGNAL(timeout()), this, SLOT(updateWaterTime()));

	pathlineTimer = new QTimer(this);
	connect(pathlineTimer, SIGNAL(timeout()), this, SLOT(recordPathPosition()));

	connect(this, SIGNAL(warning(bool)), this, SLOT(playWarning(bool)));

}

MainOGLWidget::~MainOGLWidget()
{
	// Timer
	delete this->refreshTimer;
	delete this->physicsEngineTimer;
	delete this->refreshInformationTimer;
	delete this->recordPIDWidgetTimer;
	delete this->mainROVControlJoystickTimer;
	delete this->robotArmControlJoystickTimer;
	
	delete this->waterplaneTimer;
	delete this->pathlineTimer;

	//soumnd player
	for (int i = 0; i < 2; i++) {
		delete this->bubble_player[i];
		delete this->propeller_player[i];
		delete this->warning_player[i];
	}
	
	delete this->infocouplingWidget;

	delete this->basicShader;
	delete this->compassShader;
	delete this->skyboxShader;
	delete this->marinesnowShader;
	delete this->waterplaneShader;
	delete this->pathlineShader;
	delete this->boundingboxShader;
	delete this->ballShader;

	delete this->environment_light;

	for (int i = 0; i < JoystickAmount; i++) {
		delete joystick_info[i];
	}


	delete this->rov;
	delete this->ball;
	delete this->oilplatform;
	delete this->seabed;
	delete this->waterplane;
}


// opengl callback function
void MainOGLWidget::initializeGL(void)
{
	makeCurrent();

	//  initialize glew
	if (glewInit() != GLEW_OK) {
		exit(EXIT_FAILURE);
	}

#ifdef _DEBUG
#define _DEBUG
	printf("%s\n", glGetString(GL_VERSION));
	printf("Supported GLSL version is %s.\n", (char *)glGetString(GL_SHADING_LANGUAGE_VERSION));
#endif // !_DEBUG

	// initial rov parameter and load model
	rov = new ROV();
	//rov->vdata.readData(rov->infofilepath, rov->rawfilepath);
	ball = new Model("./Model/Scene/ball/ball.obj");

	initialSound();

	initialShader();

	initialCameraMatrixUBO();

	initialSkyBox();

	initialScene();

	initialMarineSnow();

	initialEnvironmentLight();

	// some UI setting
	initialUIParameter();

	// screem & camera parameter
	initialCameraParameter();
	initialScreenViewportParameter();

	// camera menu
	initialCameraMenu();

	// texture
	initialCompassTexture();
	initialRoomTexture();

	// set time per frame;
	frameTime = 0.016666667;								// s   16.67ms = 1/60s
	PhisicsEngineTimeInterval = 0.01;						// s 
	informationRefreshTime = 0.5;							// s
	PIDInformationRecordTime = 0.01666666667;				// s
	// start timer
	refreshTimer->start(frameTime * 1000);					// fps = 60
	physicsEngineTimer->start(PhisicsEngineTimeInterval * 1000);
	refreshInformationTimer->start(informationRefreshTime * 1000);				// refresh information per 0.5s
	recordPIDWidgetTimer->start(PIDInformationRecordTime * 1000);
	waterplaneTimer->start(10);

	joystick_info[Joystick1] = new joyinfoex_tag();
	joystick_info[Joystick2] = new joyinfoex_tag();
	old_button_amount[Joystick1] = 0;
	old_button_amount[Joystick2] = 0;
	
	// PID record widget 
	this->ui.Position_X_PIDWidget->setTimeInterval(PIDInformationRecordTime);
	this->ui.Position_Y_PIDWidget->setTimeInterval(PIDInformationRecordTime);
	this->ui.Position_Z_PIDWidget->setTimeInterval(PIDInformationRecordTime);

	this->ui.Orientation_Pitch_PIDWidget->setTimeInterval(PIDInformationRecordTime);
	this->ui.Orientation_Yaw_PIDWidget->setTimeInterval(PIDInformationRecordTime);
	this->ui.Orientation_Roll_PIDWidget->setTimeInterval(PIDInformationRecordTime);

	this->ui.Velocity_Sway_PIDWidget->setTimeInterval(PIDInformationRecordTime);
	this->ui.Velocity_Sway_PIDWidget->setY_width(2.5);
	this->ui.Velocity_Heave_PIDWidget->setTimeInterval(PIDInformationRecordTime);
	this->ui.Velocity_Heave_PIDWidget->setY_width(2.5);
	this->ui.Velocity_Surge_PIDWidget->setTimeInterval(PIDInformationRecordTime);
	this->ui.Velocity_Surge_PIDWidget->setY_width(2.5);

	this->ui.AngularVelocity_Pitch_PIDWidget->setTimeInterval(PIDInformationRecordTime);
	this->ui.AngularVelocity_Yaw_PIDWidget->setTimeInterval(PIDInformationRecordTime);
	this->ui.AngularVelocity_Roll_PIDWidget->setTimeInterval(PIDInformationRecordTime);

	this->infocouplingWidget->setTimeInterval(PIDInformationRecordTime);

	// start to sound
	startBackgroundSound();
}

void MainOGLWidget::paintGL(void)
{
	makeCurrent();

	// ui Geometry setting
	// set Information group box position
	this->ui.InformationWidget->setGeometry(0, this->height() - browser_height, dividerPosition, browser_height);
	// set Divider position
	this->ui.Divider_V->setGeometry(dividerPosition, 0, 1, this->height());
	this->ui.Divider_H1->setGeometry(dividerPosition, this->height() * 3.0 / 4.0, subScreenWidth, 1);
	this->ui.Divider_H2->setGeometry(dividerPosition, this->height() * 2.0 / 4.0, subScreenWidth, 1);
	this->ui.Divider_H3->setGeometry(dividerPosition, this->height() / 4.0, subScreenWidth, 1);

	// foggy color
	if (scene_type <2) {
		foggyColor = glm::vec4(0.098f, 0.34f, 0.525f, 0.32);
	}
	else {
		foggyColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
	}

	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(foggyColor.r, foggyColor.g, foggyColor.b, foggyColor.a);
	glClearDepth(1);
	
	calculateCameraPameter();

	for (int i = 0; i < SCREENAMOUNT; i++) {
		paintScreen(i);
	}
}

void MainOGLWidget::resizeGL(int width, int height)
{
	makeCurrent();

	calculateScreenViewportParameter();
	calculateCameraPameter();
}

void MainOGLWidget::mousePressEvent(QMouseEvent* e)
{
	makeCurrent();

	switch (e->button()) {
	case Qt::LeftButton:

		break;
	case Qt::RightButton:
		change_camera_pos = glm::vec2(e->x(), e->y());
		break;
	case Qt::MidButton:

		break;
	default:
		break;
	}

}

void MainOGLWidget::mouseMoveEvent(QMouseEvent* e)
{
	makeCurrent();

	this->setFocus();
	if(e->x()<dividerPosition && e->y() < this->height() - screenViewport[0][1] && camera[screenCameraID[0]].camera_type == Free && show_cursor == false) 
	{
		this->setCursor(Qt::BlankCursor);
		QPointF widgetPos = mapToGlobal(pos());
		this->cursor().setPos(widgetPos.x() + dividerPosition / 2.0, widgetPos.y() + (this->height() - screenViewport[0][1]) / 2.0);
		camera[screenCameraID[0]].mouseMove(e->x() - dividerPosition / 2.0, (this->height() - screenViewport[0][1]) / 2.0 - e->y());
	}
	else 
	{
		this->setCursor(Qt::ArrowCursor);
	}

}

void MainOGLWidget::mouseReleaseEvent(QMouseEvent * e)
{
	makeCurrent();

	switch (e->button()) {
		case Qt::LeftButton:
			break;
		case Qt::RightButton:
			function_menu->clear();
			if (e->y() < this->height() - screenViewport[0][1] && e->x() < dividerPosition) 
			{
				function_menu->addMenu(main_screen_camera_menu);
			}
			if (e->x() > dividerPosition)
			{
				function_menu->addMenu(sub_screen_camera_menu);
			}
			function_menu->exec(e->globalPos());
			break;
		case Qt::MidButton:
			break;
		default:
			break;
	}
	update();
}

void MainOGLWidget::wheelEvent(QWheelEvent * e)
{
	makeCurrent();

	if (e->x() < dividerPosition && e->y() < this->height() - screenViewport[0][1]) {
		if (camera[screenCameraID[0]].camera_type == Axis_Fallow) {
			camera[screenCameraID[0]].setWidth(camera[screenCameraID[0]].width -= e->delta() / 50.0);
		}
	}
	else if (e->y() < screenViewport[3][1]) {
		if (camera[screenCameraID[1]].camera_type == Axis_Fallow) {
			camera[screenCameraID[1]].setWidth(camera[screenCameraID[1]].width -= e->delta() / 50.0);
		}
	}
	else if (e->y() < screenViewport[2][1]) {
		if (camera[screenCameraID[2]].camera_type == Axis_Fallow) {
			camera[screenCameraID[2]].setWidth(camera[screenCameraID[2]].width -= e->delta() / 50.0);
		}
	}
	else if (e->y() < screenViewport[1][1]) {
		if (camera[screenCameraID[3]].camera_type == Axis_Fallow) {
			camera[screenCameraID[3]].setWidth(camera[screenCameraID[3]].width -= e->delta() / 50.0);
		}
	}
	else {
		if (camera[screenCameraID[4]].camera_type == Axis_Fallow) {
			camera[screenCameraID[4]].setWidth(camera[screenCameraID[4]].width -= e->delta() / 50.0);
		}
	}

}

void MainOGLWidget::keyPressEvent(QKeyEvent * e)
{
	makeCurrent();

	QPointF widgetPos = mapToGlobal(pos());
	if (this->cursor().pos().x() < widgetPos.x() + dividerPosition &&
		this->cursor().pos().y() < widgetPos.y() + (this->height() - screenViewport[0][1]) &&
		camera[screenCameraID[0]].camera_type == Free) 
	{
		if (e->modifiers() == Qt::ControlModifier) {
			show_cursor = true;
		}
		if (e->key() == Qt::Key_W) {
			camera[screenCameraID[0]].keyboardMove(FORWARD);
		}
		if (e->key() == Qt::Key_S) {
			camera[screenCameraID[0]].keyboardMove(BACKWARD);
		}
		if (e->key() == Qt::Key_A) {
			camera[screenCameraID[0]].keyboardMove(LEFT);
		}
		if (e->key() == Qt::Key_D) {
			camera[screenCameraID[0]].keyboardMove(RIGHT);
		}
	}
}

void MainOGLWidget::keyReleaseEvent(QKeyEvent *e) 
{
	makeCurrent();

	if (e->modifiers() == Qt::NoModifier) {
		show_cursor = false;
	}




}


// my render method
/*initial function*/
void MainOGLWidget::setupOpenGL(void)
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

void MainOGLWidget::reSetEnvironment(void)
{
	makeCurrent();
	this->refreshTimer->stop();
	this->physicsEngineTimer->stop();
	this->refreshInformationTimer->stop();
	this->recordPIDWidgetTimer->stop();
	
	
	this->rov->resetLocation();

	initialScene();

	initialEnvironmentLight();

	refreshTimer->start(frameTime * 1000);					
	physicsEngineTimer->start(PhisicsEngineTimeInterval * 1000);
	refreshInformationTimer->start(informationRefreshTime * 1000);
	recordPIDWidgetTimer->start(PIDInformationRecordTime * 1000);
}

void MainOGLWidget::initialShader(void)
{	// load shader
	// compute shader
	//computeShader = new ComputeShader("./Shaders/ComputeShaderTest.cs");

	// basic shader 
	basicShader = new Shader("./Shaders/basic.vs", "./Shaders/basic.fs");

	// compass shader 
	compassShader = new Shader("./Shaders/compass.vs", "./Shaders/compass.fs");

	// skybox shader
	skyboxShader = new Shader("./Shaders/skybox.vs","./Shaders/skybox.fs");

	// sea snow Shader
	marinesnowShader = new Shader("./Shaders/snow.vs", "./Shaders/snow.fs");

	// water plane Shader
	waterplaneShader = new Shader("./Shaders/waterplane.vs", "./Shaders/waterplane.fs");

	// Path Line Shader
	pathlineShader = new Shader("./Shaders/pathline.vs", "./Shaders/pathline.fs");

	// Bounding Box Shader
	boundingboxShader = new Shader("./Shaders/boundingbox.vs", "./Shaders/boundingbox.fs");

	// Ball Shader
	ballShader = new Shader("./Shaders/ball.vs", "./Shaders/ball.fs");
}

void MainOGLWidget::initialCameraMatrixUBO(void)
{
	// configure a uniform buffer object
	// ---------------------------------
	// first. We get the relevant block indices
	unsigned int basicShader_uniformBlockIndex = glGetUniformBlockIndex(basicShader->ID, "Matrices");
	unsigned int skyboxShader_uniformBlockIndex= glGetUniformBlockIndex(skyboxShader->ID, "Matrices");
	unsigned int marinesnowShader_uniformBlockIndex = glGetUniformBlockIndex(marinesnowShader->ID, "Matrices");
	unsigned int waterplaneShader_uniformBlockIndex = glGetUniformBlockIndex(waterplaneShader->ID, "Matrices");
	unsigned int pathlineShader_uniformBlockIndex = glGetUniformBlockIndex(pathlineShader->ID, "Matrices");
	unsigned int boundingboxShader_uniformBlockIndex = glGetUniformBlockIndex(boundingboxShader->ID, "Matrices");
	unsigned int ballShader_uniformBlockIndex = glGetUniformBlockIndex(ballShader->ID, "Matrices");
	// then we link each shader's uniform block to this uniform binding point
	glUniformBlockBinding(basicShader->ID, basicShader_uniformBlockIndex, 0);
	glUniformBlockBinding(skyboxShader->ID, skyboxShader_uniformBlockIndex, 0);
	glUniformBlockBinding(marinesnowShader->ID, marinesnowShader_uniformBlockIndex, 0);
	glUniformBlockBinding(waterplaneShader->ID, waterplaneShader_uniformBlockIndex, 0);
	glUniformBlockBinding(pathlineShader->ID, pathlineShader_uniformBlockIndex, 0);
	glUniformBlockBinding(boundingboxShader->ID, boundingboxShader_uniformBlockIndex, 0);
	glUniformBlockBinding(ballShader->ID, ballShader_uniformBlockIndex, 0);
	// Now actually create the buffer
	glGenBuffers(1, &this->cameraMatrixUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, cameraMatrixUBO);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	// define the range of the buffer that links to a uniform binding point
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, cameraMatrixUBO, 0, 2 * sizeof(glm::mat4));
}

void MainOGLWidget::initialEnvironmentLight(void)
{
	if (!environment_light) {
		if (scene_type < 2) {
			environment_light = new Light(DIRECTIONLIGHT);
		}
		else {
			environment_light = new Light(POINTLIGHT);
		}
	}
	else {
		delete environment_light;
		if (scene_type < 2) {
			environment_light = new Light(DIRECTIONLIGHT);
		}
		else {
			environment_light = new Light(POINTLIGHT);
		}
	}
	if (scene_type < 2) {
		environment_light->setDirection(glm::vec4(this->environment_light_direction, 0.0));
		environment_light->setAmbient(glm::vec4(this->environment_light_ambient, 1.0));
		environment_light->setDiffuse(glm::vec4(this->environment_light_diffuse, 1.0));
		environment_light->setSpecular(glm::vec4(this->environment_light_specular, 1.0));
		environment_light->setAttenuation_Constant(this->environment_light_attenuation.x);
		environment_light->setAttenuation_Linear(this->environment_light_attenuation.y);
		environment_light->setAttenuation_Quadratic(this->environment_light_attenuation.z);
		environment_light->populateAllParameter(basicShader);
	}
	else {
		environment_light->setPosition(glm::vec4(0.0f, 10.0f, 0.0f, 0.0f));
		environment_light->setAmbient(glm::vec4(this->environment_light_ambient, 1.0));
		environment_light->setDiffuse(glm::vec4(this->environment_light_diffuse, 1.0));
		environment_light->setSpecular(glm::vec4(this->environment_light_specular, 1.0));
		environment_light->setAttenuation_Constant(1.0);
		environment_light->setAttenuation_Linear(0.0);
		environment_light->setAttenuation_Quadratic(0.0);
		environment_light->populateAllParameter(basicShader);
	}
}

void MainOGLWidget::initialScene(void)
{
	// water plane
	waterplane = new MyWaterPlane(100, 100, 0.1);
	waterplane->genWaterPlane(*waterplaneShader);
	waterplane->genSinWave(10, 0.6, 5, glm::normalize(glm::vec2(1.0, 1.0)));
	waterplane->genSinWave(10, 0.4, 1, glm::normalize(glm::vec2(-1.0, 1.0)));
	waterplane->populateSinWaveParameter(*waterplaneShader);

	basicShader->use();

	if (terrain_scene_loaded) {
		delete seabed;
		terrain_scene_loaded = false;
	}

	if (scene_type == Sand) {
		seabed = new Terrain("./Model/Scene/underwater/terrain_sand.obj", "./Model/Scene/ball/ball.obj", 0.3, Sand);
		seabed->translate_terrain_position(0, -10, 0);
		terrain_scene_loaded = true;
	}else if (scene_type == Rock) {
		seabed = new Terrain("./Model/Scene/underwater/terrain_rock.obj", "./Model/Scene/ball/ball.obj", 0.3, Rock);
		seabed->translate_terrain_position(0, -40, 0);
		terrain_scene_loaded = true;
	}else if(scene_type == 2){
		seabed = new Terrain("./Model/Scene/underwater/terrain_sand.obj", "./Model/Scene/ball/ball.obj", 0.3, Sand);
		seabed->translate_terrain_position(0, -10, 0);
		terrain_scene_loaded = true;
	}

	oilplatform = new Oilplatform();

}

void MainOGLWidget::initialMarineSnow(void)
{
	// initial seasnow
	marinesnow = new MarineSnow();
}

void MainOGLWidget::initialSkyBox(void)
{
	string imagepath[] = { 
		"./Image/Scene/BackScreen_Side.png",
		"./Image/Scene/BackScreen_Side.png",
		"./Image/Scene/BackScreen_Top.png",
		"./Image/Scene/BackScreen_Bottom.png",
		"./Image/Scene/BackScreen_Side.png",
		"./Image/Scene/BackScreen_Side.png"
	};
	cubemap = loadCubeMap(imagepath);
	if (cubemap != -1) {
		skyBoxVAO = initialSkyBoxVAO();
		loadskybox = true;
	}
	else {
		loadskybox = false;
	}
}

void MainOGLWidget::initialUIParameter(void)
{
	show_cursor = false;
	browser_height = 250;

	maxSubScreenWidth = (GetSystemMetrics(SM_CXSCREEN) - 210) / 3.0;


	// Initial Mode
	mode_manu = true;
	mode_joystick = false;


	// Manu Mode setting
	this->ui.position_X_Input->setReadOnly(false);
	this->ui.position_Y_Input->setReadOnly(false);
	this->ui.position_Z_Input->setReadOnly(false);
	//this->ui.orientation_Pitch_Input->setReadOnly(false);
	this->ui.orientation_Yaw_Input->setReadOnly(false);
	this->ui.orientation_Roll_Input->setReadOnly(false);

	QDoubleValidator* positionDoubleValidator = new MyValidator(-1000.000, 1000.000, 3, this);
	positionDoubleValidator->setNotation(QDoubleValidator::StandardNotation);
	QDoubleValidator* orientationDoubleValidator = new MyValidator(-179.999, 179.999, 3, this);
	orientationDoubleValidator->setNotation(QDoubleValidator::StandardNotation);
	this->ui.position_X_Input->setValidator(positionDoubleValidator);
	this->ui.position_Y_Input->setValidator(positionDoubleValidator);
	this->ui.position_Z_Input->setValidator(positionDoubleValidator);
	this->ui.orientation_Yaw_Input->setValidator(orientationDoubleValidator);
	this->ui.orientation_Roll_Input->setValidator(orientationDoubleValidator);

	global_target_position = glm::vec3(0.0f);
	target_orientation = glm::vec3(0.0f);
	target_velocity = glm::vec3(0.0f);
	target_angular_velocity = glm::vec3(0.0f);
}

void MainOGLWidget::initialCompassTexture(void)
{
	makeCurrent();
	compassShader->use();
	glEnable(GL_TEXTURE_2D);
	/*
	// old code use loadpng.cpp

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(2, texture_compass);

	std::vector<unsigned char> image;
	unsigned width, height;
	// load compass texture
	unsigned error = lodepng::decode(image, width, height, "Image/compass_1.png");

	// If there's an error, display it.
	if (error != 0)
	{
		std::cout << "error " << error << ": " << lodepng_error_text(error) << std::endl;
		return;
	}

	// Texture size must be power of two for the primitive OpenGL version this is written for. Find next power of two.
	size_t u2 = 1; while (u2 < width) u2 *= 2;
	size_t v2 = 1; while (v2 < height) v2 *= 2;
	// Ratio for power of two version compared to actual version, to render the non power of two image with proper size.
	double u3 = (double)width / u2;
	double v3 = (double)height / v2;

	// Make power of two version of the image.
	std::vector<unsigned char> compassImage_1(u2 * v2 * 4);
	for (size_t y = 0; y < height; y++)
		for (size_t x = 0; x < width; x++)
			for (size_t c = 0; c < 4; c++)
			{
				compassImage_1[4 * u2 * y + 4 * x + c] = image[4 * width * y + 4 * x + c];
			}
	

	glBindTexture(GL_TEXTURE_2D, texture_compass[0]);
	glGenerateMipmap(texture_compass[0]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //GL_NEAREST = no smoothing
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, u2, v2, 0, GL_RGBA, GL_UNSIGNED_BYTE, &compassImage_1[0]);

	image.clear();
	error = lodepng::decode(image, width, height, "Image/compass_2.png");

	// If there's an error, display it.
	if (error != 0)
	{
		std::cout << "error " << error << ": " << lodepng_error_text(error) << std::endl;
		return;
	}

	// Texture size must be power of two for the primitive OpenGL version this is written for. Find next power of two.
	u2 = 1; while (u2 < width) u2 *= 2;
	v2 = 1; while (v2 < height) v2 *= 2;
	// Ratio for power of two version compared to actual version, to render the non power of two image with proper size.
	u3 = (double)width / u2;
	v3 = (double)height / v2;

	// Make power of two version of the image.
	std::vector<unsigned char> compassImage_2(u2 * v2 * 4);
	for (size_t y = 0; y < height; y++)
		for (size_t x = 0; x < width; x++)
			for (size_t c = 0; c < 4; c++)
			{
				compassImage_2[4 * u2 * y + 4 * x + c] = image[4 * width * y + 4 * x + c];
			}

	
	glBindTexture(GL_TEXTURE_2D, texture_compass[1]);
	glGenerateMipmap(texture_compass[1]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //GL_NEAREST = no smoothing
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, u2, v2, 0, GL_RGBA, GL_UNSIGNED_BYTE, &compassImage_2[0]);



	// load rov texture
	glGenTextures(3, texture_ROV);
	// rov forward 
	image.clear();
	error = lodepng::decode(image, width, height, "Image/submarien-backward.png");

	// If there's an error, display it.
	if (error != 0)
	{
		std::cout << "error " << error << ": " << lodepng_error_text(error) << std::endl;
		return;
	}

	// Texture size must be power of two for the primitive OpenGL version this is written for. Find next power of two.
	u2 = 1; while (u2 < width) u2 *= 2;
	v2 = 1; while (v2 < height) v2 *= 2;
	// Ratio for power of two version compared to actual version, to render the non power of two image with proper size.
	u3 = (double)width / u2;
	v3 = (double)height / v2;

	// Make power of two version of the image.
	std::vector<unsigned char> rovImage_forward(u2 * v2 * 4);
	for (size_t y = 0; y < height; y++)
		for (size_t x = 0; x < width; x++)
			for (size_t c = 0; c < 4; c++)
			{
				rovImage_forward[4 * u2 * y + 4 * x + c] = image[4 * width * y + 4 * x + c];
			}


	glBindTexture(GL_TEXTURE_2D, texture_ROV[0]);
	glGenerateMipmap(texture_ROV[0]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //GL_NEAREST = no smoothing
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, u2, v2, 0, GL_RGBA, GL_UNSIGNED_BYTE, &rovImage_forward[0]);

	// rov forward 
	image.clear();
	error = lodepng::decode(image, width, height, "Image/submarien-startboard.png");

	// If there's an error, display it.
	if (error != 0)
	{
		std::cout << "error " << error << ": " << lodepng_error_text(error) << std::endl;
		return;
	}

	// Texture size must be power of two for the primitive OpenGL version this is written for. Find next power of two.
	u2 = 1; while (u2 < width) u2 *= 2;
	v2 = 1; while (v2 < height) v2 *= 2;
	// Ratio for power of two version compared to actual version, to render the non power of two image with proper size.
	u3 = (double)width / u2;
	v3 = (double)height / v2;

	// Make power of two version of the image.
	std::vector<unsigned char> rovImage_left(u2 * v2 * 4);
	for (size_t y = 0; y < height; y++)
		for (size_t x = 0; x < width; x++)
			for (size_t c = 0; c < 4; c++)
			{
				rovImage_left[4 * u2 * y + 4 * x + c] = image[4 * width * y + 4 * x + c];
			}


	glBindTexture(GL_TEXTURE_2D, texture_ROV[1]);
	glGenerateMipmap(texture_ROV[1]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //GL_NEAREST = no smoothing
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, u2, v2, 0, GL_RGBA, GL_UNSIGNED_BYTE, &rovImage_left[0]);

	// rov forward 
	image.clear();
	error = lodepng::decode(image, width, height, "Image/submarien-top.png");

	// If there's an error, display it.
	if (error != 0)
	{
		std::cout << "error " << error << ": " << lodepng_error_text(error) << std::endl;
		return;
	}

	// Texture size must be power of two for the primitive OpenGL version this is written for. Find next power of two.
	u2 = 1; while (u2 < width) u2 *= 2;
	v2 = 1; while (v2 < height) v2 *= 2;
	// Ratio for power of two version compared to actual version, to render the non power of two image with proper size.
	u3 = (double)width / u2;
	v3 = (double)height / v2;

	// Make power of two version of the image.
	std::vector<unsigned char> rovImage_up(u2 * v2 * 4);
	for (size_t y = 0; y < height; y++)
		for (size_t x = 0; x < width; x++)
			for (size_t c = 0; c < 4; c++)
			{
				rovImage_up[4 * u2 * y + 4 * x + c] = image[4 * width * y + 4 * x + c];
			}


	glBindTexture(GL_TEXTURE_2D, texture_ROV[2]);
	glGenerateMipmap(texture_ROV[2]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //GL_NEAREST = no smoothing
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, u2, v2, 0, GL_RGBA, GL_UNSIGNED_BYTE, &rovImage_up[0]);
	*/

	texture_compass[0] = TextureFromFile("compass_1.png", "./Image");
	texture_compass[1] = TextureFromFile("compass_2.png", "./Image");
	texture_ROV[0] = TextureFromFile("submarien-backward.png", "./Image"); 
	texture_ROV[1] = TextureFromFile("submarien-startboard.png", "./Image");
	texture_ROV[2] = TextureFromFile("submarien-top.png", "./Image");
}

void MainOGLWidget::initialRoomTexture(void)
{
	makeCurrent();
	int checkImageHeight = 128, checkImageWidth = 128;
	static GLubyte checkImage[128][128][4];
	int i, j, c;
	int gap = 1;

	for (i = 0; i < checkImageHeight; i++) {
		for (j = 0; j < checkImageWidth; j++) {
			c = ((((i & gap) == 0) ^ ((j & gap)) == 0)) * 255;
			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
			checkImage[i][j][3] = (GLubyte)255;
		}
	}

	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &texture_checkerboard);

	glBindTexture(GL_TEXTURE_2D, texture_checkerboard);
	glGenerateMipmap(texture_checkerboard);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //GL_NEAREST = no smoothing
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, checkImageHeight, checkImageWidth, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);


}

void MainOGLWidget::initialScreenViewportParameter(void)
{
	makeCurrent();
	// initial only once

	// To provide user a function that can select camera on the main screen and sub screen
	// initial each screen's camera id 
	// initial value: 
	//		screen 0 (main)		camera 0 (main)
	//		sub screen 1		camera 1
	//		sub screen 2		camera 2
	//		sub screen 3		camera 3
	//		sub screen 4		camera 4
	// change screenCameraID value can change the camera on the each screen
	screenCameraID[0] = total_camera_amount - 1;
	for (int i = 1; i < SCREENAMOUNT; i++) {
		screenCameraID[i] = i - 1;
	}


	// position
	// main screen position is fixed
	screenViewport[0][0] = 0;
	screenViewport[0][1] = browser_height;
	
	for (int i = 1; i < SCREENAMOUNT; i++) {
		screenViewport[i][0] = dividerPosition;
		screenViewport[i][1] = this->height() * (4 - i) / 4;
	}

	// height width
	dividerPosition = this->width() * 2.0 / 3.0;
	subScreenWidth = this->width() - dividerPosition;

	for (int i = 1; i < SCREENAMOUNT; i++) {
		screenViewport[i][2] = subScreenWidth;
	}

	for (int i = 1; i < SCREENAMOUNT; i++) {
		screenViewport[i][3] = this->height() / 4.0;
	}

	screenViewport[0][2] = dividerPosition;
	screenViewport[0][3] = this->height() - browser_height;
	
}

void MainOGLWidget::initialCameraParameter(void)
{
	makeCurrent();
	glm::vec3 position, lookat, lookup;

	glm::vec4 axis_lookat[6] = {
		glm::vec4(-1.0, 0.0, 0.0, 0.0),
		glm::vec4(1.0, 0.0, 0.0, 0.0),
		glm::vec4(0.0, -1.0, 0.0, 0.0),
		glm::vec4(0.0, 1.0, 0.0, 0.0),
		glm::vec4(0.0, 0.0, -1.0, 0.0),
		glm::vec4(0.0, 0.0, 1.0, 0.0)
	};

	glm::vec4 axis_lookup[6] = {
		glm::vec4(0.0, 1.0, 0.0, 0.0),
		glm::vec4(0.0, 1.0, 0.0, 0.0),
		glm::vec4(0.0, 0.0, 1.0, 0.0),
		glm::vec4(0.0, 0.0, 1.0, 0.0),
		glm::vec4(0.0, 1.0, 0.0, 0.0),
		glm::vec4(0.0, 1.0, 0.0, 0.0)
	};

	// 6 axis camera + 1 free camera
	total_camera_amount = rov->camera_amount + 6 + 1;

	camera = new Camera_t[total_camera_amount];

	// Initial rov camera
	for (int i = 0; i < rov->camera_amount; i++) {
		camera[i].camera_type = ROV_Camera;
	}

	// Initial axis fallow camera
	for (int i = rov->camera_amount; i < rov->camera_amount + 6; i++) 
	{
		camera[i].camera_type = Axis_Fallow;
		camera[i].setWidth(50.0);
		camera[i].setFrontVector(axis_lookat[i - rov->camera_amount].x, axis_lookat[i - rov->camera_amount].y, axis_lookat[i - rov->camera_amount].z);
		camera[i].setViewUpVector(axis_lookup[i - rov->camera_amount].x, axis_lookup[i - rov->camera_amount].y, axis_lookup[i - rov->camera_amount].z);
	}

	// Initial free camera
	camera[total_camera_amount - 1].camera_type = Free;
	position = rov->total_displacement + glm::vec3(0.0, 0.0, 5.0);
	camera[total_camera_amount - 1].setPositionPoint(position.x, position.y, position.z);
	camera[total_camera_amount - 1].setFrontVector(0.0, 0.0, -1.0);
	camera[total_camera_amount - 1].setViewUpVector(0.0, 1.0, 0.0);
	camera[total_camera_amount - 1].setRightVector(1.0, 0.0, 0.0);
}

void MainOGLWidget::initialCameraMenu(void)
{
	makeCurrent();
	function_menu = new QMenu("Function", this);
	main_screen_camera_menu = new QMenu("Camera", this);
	sub_screen_camera_menu = new QMenu("Camera", this);
	connect(main_screen_camera_menu, SIGNAL(triggered(QAction*)), this, SLOT(changeCamera(QAction*)));
	connect(sub_screen_camera_menu, SIGNAL(triggered(QAction*)), this, SLOT(changeCamera(QAction*)));

	QString action_name[] = {
		QString("ROV main camera"),
		QString("ROV sub camera 1"),
		QString("ROV sub camera 2"),
		QString("ROV sub camera 3"),
		QString("Axis X camera"),
		QString("Axis -X camera"),
		QString("Axis Y camera"),
		QString("Axis -Y camera"),
		QString("Axis Z camera"),
		QString("Axis -Z camera"),
		QString("Free camera")
	};

	QAction** act;
	act = new QAction*[total_camera_amount];

	for (int i = 0; i < total_camera_amount; i++) {
		act[i] = new QAction(action_name[i]);
		act[i]->setData(i);
	}

	main_screen_camera_menu->clear();
	for (int i = 0; i < total_camera_amount; i++) {
		main_screen_camera_menu->addAction(act[i]);
	}


	sub_screen_camera_menu->clear();
	for (int i = 0; i < total_camera_amount - 1; i++) {
		sub_screen_camera_menu->addAction(act[i]);
	}

}

void MainOGLWidget::initialPathLine(void)
{
	makeCurrent();
	glGenVertexArrays(1, &pathLineVAO);
	glGenBuffers(1, &pathLineVBO);
}

/*Calculate function*/
void MainOGLWidget::calculateScreenViewportParameter(void)
{
	makeCurrent();
	if (this->width() / 3.0 < maxSubScreenWidth) {
		dividerPosition = this->width() * 2.0 / 3.0;
		subScreenWidth = this->width() - dividerPosition;
	}
	else {
		subScreenWidth = maxSubScreenWidth;
	}

	// main screen only need to change height and width
	screenViewport[0][2] = dividerPosition = this->width() - subScreenWidth;
	screenViewport[0][3] = this->height() - browser_height;

	for (int i = 1; i < SCREENAMOUNT; i++) {
		screenViewport[i][0] = screenViewport[i][0] = dividerPosition;
		screenViewport[i][1] = this->height() * (4 - i) / 4;
		screenViewport[i][2] = subScreenWidth;
		screenViewport[i][3] = this->height() / 4.0;
	}
}

void MainOGLWidget::calculateCameraPameter(void)
{
	makeCurrent();
	glm::vec3 position, lookat, lookup;

	glm::vec3 axis_position[6] = {
		glm::vec3(3.0, 0.0, 0.0),
		glm::vec3(-3.0, 0.0, 0.0),
		glm::vec3(0.0, 3.0, 0.0),
		glm::vec3(0.0, -3.0, 0.0),
		glm::vec3(0.0, 0.0, 3.0),
		glm::vec3(0.0, 0.0, -3.0)
	};

	// Update rov camera
	for (int i = 0; i < rov->camera_amount; i++) {
		position = rov->modelMatrix * rov->camera_position[i];
		lookat = rov->RotateMatrix * rov->look_at[i];
		lookup = rov->RotateMatrix * rov->look_up[i];
		camera[i].setPositionPoint(position.x, position.y, position.z);
		camera[i].setFrontVector(lookat.x, lookat.y, lookat.z);
		camera[i].setViewUpVector(lookup.x, lookup.y, lookup.z);
	}

	// Update axis fallow camera
	for (int i = rov->camera_amount; i < rov->camera_amount + 6; i++) {
		position = rov->total_displacement + axis_position[i - rov->camera_amount];
		//position = glm::vec3(0.0, 2.0, 0.0) + axis_position[i - rov->camera_amount];
		camera[i].setPositionPoint(position.x, position.y, position.z);
	}

	// Update free camera
	camera[total_camera_amount - 1].updateCameraVectors();

}


/*draw funciton*/
void MainOGLWidget::paintScreen(int screenNumber)
{
	makeCurrent();
	glm::mat4 viewMatrix, projMatrix;
	glm::vec3 viewPos;

	glViewport(screenViewport[screenNumber][0], screenViewport[screenNumber][1], screenViewport[screenNumber][2], screenViewport[screenNumber][3]);
	
	// set Aspect
	camera[screenCameraID[screenNumber]].setAspect(screenViewport[screenNumber][2], screenViewport[screenNumber][3]);


	// populate data to shader
	// build view matrix and projection matrix then send to shader
	viewMatrix = camera[screenCameraID[screenNumber]].lookAt();
	if (camera[screenCameraID[screenNumber]].camera_type == Axis_Fallow) {
		projMatrix = camera[screenCameraID[screenNumber]].ortho();
	}
	else {
		projMatrix = camera[screenCameraID[screenNumber]].perspective();
	}

	// set projection matrix and view matrix into ubo
	glBindBuffer(GL_UNIFORM_BUFFER, cameraMatrixUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projMatrix));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(viewMatrix));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	basicShader->use();
	viewPos = camera[screenCameraID[screenNumber]].position;
	basicShader->setVec3("viewPos", viewPos);
	basicShader->setVec4("foggyColor", foggyColor);

	// light parameter
	basicShader->setInt("num_PointLight", Light::point_light_amount);
	basicShader->setInt("num_DirectLight", Light::direct_light_amount);
	basicShader->setInt("num_SpotLight", Light::spot_light_amount);
	// ROV spot light
	for (int i = 0; i < rov->spotlight_amount; i++) {
		rov->light[i].setPosition(rov->modelMatrix * rov->light_position[i]);
		rov->light[i].setDirection(rov->modelMatrix * rov->light_direction[i]);
		rov->light[i].setAmbient(glm::vec4(0.0, 0.0, 0.0, 1.0));
		rov->light[i].setDiffuse(glm::vec4(0.3, 0.3, 0.3, 1.0));
		rov->light[i].setSpecular(glm::vec4(0.1, 0.1, 0.1, 1.0));
		rov->light[i].populateAllParameter(basicShader);
	}
	

	/*------------------------Draw stuff----------------------------------*/
	
	paintROV();

	paintScene();

	//paintWaterPlane();

	if (marinesnow_use) 
	{
		paintMarineSnow(viewPos);
	}

	if (showpathline) {
		paintROVPath();
	}

	if (screenNumber == 0) {
		paintCompass();
	}
}

void MainOGLWidget::paintCompass(void)
{
	makeCurrent();
	glm::mat4 modelMatrix;

	compassShader->use();
	glDisable(GL_DEPTH_TEST);
	glViewport(screenViewport[0][0], screenViewport[0][1], screenViewport[0][2], screenViewport[0][3]);
	
	float w = screenViewport[0][2];
	float h = screenViewport[0][3];

	compassShader->setFloat("width", w);
	compassShader->setFloat("height", h);

	float billboard_lenth = w / 5.0;

	// vertice are all in clip space
	glm::vec2 BillBoard[6]{
		glm::vec2{ -billboard_lenth / 2.0 ,	-billboard_lenth / 2.0 },
		glm::vec2{  billboard_lenth / 2.0 ,	-billboard_lenth / 2.0 },
		glm::vec2{  billboard_lenth / 2.0 ,	 billboard_lenth / 2.0 },
		glm::vec2{ -billboard_lenth / 2.0 ,	-billboard_lenth / 2.0 },
		glm::vec2{  billboard_lenth / 2.0 ,	 billboard_lenth / 2.0 },
		glm::vec2{ -billboard_lenth / 2.0 ,	 billboard_lenth / 2.0 }
	};

	const float planeTexCoor[6][2]{
		{ 0.0,1.0 },
		{ 1.0,1.0 },
		{ 1.0,0.0 },
		{ 0.0,1.0 },
		{ 1.0,0.0 },
		{ 0.0,0.0 }
	};

	// 
	GLuint compass_Vertice_Buffer;
	glGenBuffers(1, &compass_Vertice_Buffer);
	glBindBuffer(GL_ARRAY_BUFFER, compass_Vertice_Buffer);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), BillBoard, GL_STATIC_DRAW);
	
	GLuint compass_Texcoor_Buffer;
	glGenBuffers(1, &compass_Texcoor_Buffer);
	glBindBuffer(GL_ARRAY_BUFFER, compass_Texcoor_Buffer);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), planeTexCoor, GL_STATIC_DRAW);

	// Gen BillBoard VAO
	GLuint BillBoardVAO;
	glGenVertexArrays(1, &BillBoardVAO);
	// Bind compass 1 VAO
	glBindVertexArray(BillBoardVAO);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, compass_Vertice_Buffer);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, compass_Texcoor_Buffer);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindVertexArray(0);


	// Draw compass 1
	compassShader->setFloat("depth", -0.999);
	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(w, h, 0.0));
	modelMatrix = glm::translate(modelMatrix, glm::vec3(-billboard_lenth / 2.0, -billboard_lenth / 2.0, 0.0));
	compassShader->setMat4("model", modelMatrix);
	glBindTexture(GL_TEXTURE_2D, texture_compass[0]);
	glBindVertexArray(BillBoardVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	// Draw  rov_up
	compassShader->setFloat("depth", -1.0);
	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(w, h, 0.0));
	modelMatrix = glm::translate(modelMatrix, glm::vec3(-billboard_lenth / 2.0, -billboard_lenth / 2.0, 0.0));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0, 1.0, 1.0));
	modelMatrix = glm::rotate(modelMatrix, -this->rov->total_rotate_radian.y, glm::vec3(0.0, 0.0, 1.0));
	compassShader->setMat4("model", modelMatrix);
	glBindTexture(GL_TEXTURE_2D, texture_ROV[2]);
	glBindVertexArray(BillBoardVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	// Draw compass 2
	compassShader->setFloat("depth", -0.999);
	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(w, h, 0.0));
	modelMatrix = glm::translate(modelMatrix, glm::vec3(-billboard_lenth / 2.0, (-billboard_lenth / 2.0) * 3, 0.0));
	compassShader->setMat4("model", modelMatrix);
	glBindTexture(GL_TEXTURE_2D, texture_compass[1]);
	glBindVertexArray(BillBoardVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	// Draw  rov_forward
	compassShader->setFloat("depth", -1.0);
	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(w, h, 0.0));
	modelMatrix = glm::translate(modelMatrix, glm::vec3(-billboard_lenth / 2.0, (-billboard_lenth / 2.0) * 3, 0.0));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0, 1.0, 1.0));
	modelMatrix = glm::rotate(modelMatrix, -this->rov->total_rotate_radian.z, glm::vec3(0.0, 0.0, 1.0));
	compassShader->setMat4("model", modelMatrix);
	glBindTexture(GL_TEXTURE_2D, texture_ROV[0]);
	glBindVertexArray(BillBoardVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	// Draw compass 3
	compassShader->setFloat("depth", -0.999);
	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(w, h, 0.0));
	modelMatrix = glm::translate(modelMatrix, glm::vec3(-billboard_lenth / 2.0, (-billboard_lenth / 2.0) * 5, 0.0));
	compassShader->setMat4("model", modelMatrix);
	glBindTexture(GL_TEXTURE_2D, texture_compass[1]);
	glBindVertexArray(BillBoardVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	// Draw  rov_left
	compassShader->setFloat("depth", -1.0);
	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(w, h, 0.0));
	modelMatrix = glm::translate(modelMatrix, glm::vec3(-billboard_lenth / 2.0, (-billboard_lenth / 2.0) * 5, 0.0));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0, 1.0, 1.0));
	modelMatrix = glm::rotate(modelMatrix, this->rov->total_rotate_radian.x, glm::vec3(0.0, 0.0, 1.0));
	compassShader->setMat4("model", modelMatrix);
	glBindTexture(GL_TEXTURE_2D, texture_ROV[1]);
	glBindVertexArray(BillBoardVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	
	glDeleteBuffers(1, &compass_Vertice_Buffer);
	glDeleteBuffers(1, &compass_Texcoor_Buffer);
	glDeleteVertexArrays(1, &BillBoardVAO);

	glEnable(GL_DEPTH_TEST);
}

void MainOGLWidget::paintRoom(void)
{
	makeCurrent();
	glm::mat4 modelMatrix;

	basicShader->use();
	glm::vec4 checkerboard_vertices[6] = {
		glm::vec4(-64.0,-64.0,0.0,1.0),
		glm::vec4( 64.0,-64.0,0.0,1.0),
		glm::vec4( 64.0, 64.0,0.0,1.0),
		glm::vec4(-64.0,-64.0,0.0,1.0),
		glm::vec4( 64.0, 64.0,0.0,1.0),
		glm::vec4(-64.0, 64.0,0.0,1.0)
	};

	glm::vec4 checkerboard_normal[6] = {
		glm::vec4(0.0,0.0,1.0,0.0),
		glm::vec4(0.0,0.0,1.0,0.0),
		glm::vec4(0.0,0.0,1.0,0.0),
		glm::vec4(0.0,0.0,1.0,0.0),
		glm::vec4(0.0,0.0,1.0,0.0),
		glm::vec4(0.0,0.0,1.0,0.0)
	};

	glm::vec2 checkerboard_texture_coord[6] = {
		glm::vec2(-1.0,-1.0),
		glm::vec2( 1.0,-1.0),
		glm::vec2( 1.0, 1.0),
		glm::vec2(-1.0,-1.0),
		glm::vec2( 1.0, 1.0),
		glm::vec2(-1.0, 1.0)
	};


	GLuint ckeckerboard_Vertice_Buffer;
	glGenBuffers(1, &ckeckerboard_Vertice_Buffer);
	glBindBuffer(GL_ARRAY_BUFFER, ckeckerboard_Vertice_Buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(checkerboard_vertices), checkerboard_vertices, GL_STATIC_DRAW);

	GLuint ckeckerboard_Normal_Buffer;
	glGenBuffers(1, &ckeckerboard_Normal_Buffer);
	glBindBuffer(GL_ARRAY_BUFFER, ckeckerboard_Normal_Buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(checkerboard_normal), checkerboard_normal, GL_STATIC_DRAW);

	GLuint ckeckerboard_TexCoord_Buffer;
	glGenBuffers(1, &ckeckerboard_TexCoord_Buffer);
	glBindBuffer(GL_ARRAY_BUFFER, ckeckerboard_TexCoord_Buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(checkerboard_texture_coord), checkerboard_texture_coord, GL_STATIC_DRAW);

	// Gen Ckeckerboard VAO
	GLuint checkerboardVAO;
	glGenVertexArrays(1, &checkerboardVAO);
	// Bind Ckeckerboard VAO
	glBindVertexArray(checkerboardVAO);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, ckeckerboard_Vertice_Buffer);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), NULL);

	glBindBuffer(GL_ARRAY_BUFFER, ckeckerboard_Normal_Buffer);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), NULL);

	glBindBuffer(GL_ARRAY_BUFFER, ckeckerboard_TexCoord_Buffer);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), NULL);
	glBindVertexArray(0);

	// face to +z
	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -64));
	basicShader->setMat4("model", modelMatrix);
	glBindTexture(GL_TEXTURE_2D, texture_checkerboard);
	glBindVertexArray(checkerboardVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// face to -z
	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 64));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
	basicShader->setMat4("model", modelMatrix);
	glBindTexture(GL_TEXTURE_2D, texture_checkerboard);
	glBindVertexArray(checkerboardVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// face to +x
	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-64.0, 0.0, 0.0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
	basicShader->setMat4("model", modelMatrix);
	glBindTexture(GL_TEXTURE_2D, texture_checkerboard);
	glBindVertexArray(checkerboardVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// face to -x
	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(64.0, 0.0, 0.0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
	basicShader->setMat4("model", modelMatrix);
	glBindTexture(GL_TEXTURE_2D, texture_checkerboard);
	glBindVertexArray(checkerboardVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// face to +y
	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, -64.0, 0.0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
	basicShader->setMat4("model", modelMatrix);
	glBindTexture(GL_TEXTURE_2D, texture_checkerboard);
	glBindVertexArray(checkerboardVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// face to -y
	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 64.0, 0.0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
	basicShader->setMat4("model", modelMatrix);
	glBindTexture(GL_TEXTURE_2D, texture_checkerboard);
	glBindVertexArray(checkerboardVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void MainOGLWidget::paintROV(void)
{
	// main rov
	this->rov->drawMainROV(basicShader);

	/*
	// ball
	glm::mat4 modelMatrix(1.0f);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.01, 0.01, 0.01));
	basicShader->setMat4("model", modelMatrix);
	this->ball->Draw(*basicShader);
	*/

	// robot arm
	this->rov->drawRobotArm(basicShader);

	// draw bounding box
	if (showboundingbox) {
		this->rov->drawBoundingBox(boundingboxShader, 2);
	}

	// draw mass center
	//if (showphysicscenter) {
		this->rov->drawPhysicsCenter(ballShader);
	//}
}

void MainOGLWidget::paintScene(void)
{

	glm::mat4 modelMatrix, translateMatrix, rotationMatrix, scalingMatrix;
	basicShader->use();
	
	if (terrain_scene_loaded) {
		seabed->render(*basicShader, rov->total_displacement);
		oilplatform->draw(basicShader);
	}

	if (scene_type < 2 ) {
		if (loadskybox)
		{
			skyboxShader->use();
			drawSkyBox(skyBoxVAO, cubemap);
		}
	}
	else {
		paintRoom();
	}
}

void MainOGLWidget::paintWaterPlane()
{
	// water plane
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	waterplaneShader->use();
	waterplaneShader->setFloat("time", this->water_time);
	waterplane->draw(*waterplaneShader);
	glDisable(GL_BLEND);
}

void MainOGLWidget::paintMarineSnow(glm::vec3 view_position)
{
	marinesnowShader->use();
	this->marinesnow->snow_drop(view_position);
	this->marinesnow->render_marine_snow(view_position, *marinesnowShader);
}

void MainOGLWidget::paintROVPath()
{
	if (position_list.getSize() > 0) {
		pathlineShader->use();
		pathlineShader->setVec3("color", glm::vec3(1.0, 0.0, 0.0));
		vector<glm::vec3> position_vertices;
		for (int i = 0; i < position_list.getSize(); i++) {
			position_vertices.push_back(position_list[i]);
		}
		glBindVertexArray(pathLineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, pathLineVBO);
		glBufferData(GL_ARRAY_BUFFER, position_vertices.size() * sizeof(glm::vec3), &position_vertices[0], GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), NULL);
		glBindVertexArray(pathLineVAO);
		glLineWidth(2.0);
		glDrawArrays(GL_LINE_STRIP, 0, position_vertices.size());
		glBindVertexArray(0);
	}
}

void MainOGLWidget::setSceneType(int scene_type)
{
	this->scene_type = scene_type;
}

void MainOGLWidget::setMarineSnowUse(bool flag)
{
	this->marinesnow_use = flag;
}

void MainOGLWidget::setEnvironmentLightParameter(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 direction, glm::vec3 attenuation)
{
	this->environment_light_ambient = ambient;
	this->environment_light_diffuse = diffuse;
	this->environment_light_specular = specular;
	this->environment_light_direction = direction;
	this->environment_light_attenuation = attenuation;
}


// sound function
void MainOGLWidget::initialSound(void)
{
	// bubble
	for (int i = 0; i < 2; i++) {
		bubble_player[i] = new QMediaPlayer;
		bubble_player[i]->setMedia(QUrl("./Sound/bubble.mp3"));;
	}
	// propeller
	for (int i = 0; i < 2; i++) {
		propeller_player[i] = new QMediaPlayer;
		propeller_player[i]->setMedia(QUrl("./Sound/propeller.wav"));;
		propeller_player[i]->setVolume(30);
	}
	// warning
	for (int i = 0; i < 2; i++) {
		warning_player[i] = new QMediaPlayer;
		warning_player[i]->setMedia(QUrl("./Sound/warning.wav"));;
	}

	for (int i = 0; i < 2; i++) {
		warning_sound_timer[i] = new QTimer;
	}

}

void MainOGLWidget::startBackgroundSound(void)
{
	// bubble tomer
	for (int i = 0; i < 2; i++) {
		bubble_sound_timer[i] = new QTimer;
	}

	// bubble sound 
	QObject::connect(bubble_player[0], &QMediaPlayer::mediaStatusChanged,
		[&](QMediaPlayer::MediaStatus status) {
		if (status == QMediaPlayer::LoadedMedia) //when loading is finished
		{
			QObject::connect(bubble_sound_timer[0], &QTimer::timeout, [&]() {
				bubble_player[0]->stop();
				bubble_player[0]->play();
				// loop time
				bubble_sound_timer[0]->start(bubble_player[0]->duration());
			});
			bubble_sound_timer[0]->start();
		}
	});
	QObject::connect(bubble_player[1], &QMediaPlayer::mediaStatusChanged,
		[&](QMediaPlayer::MediaStatus status) {
		if (status == QMediaPlayer::LoadedMedia) //when loading is finished
		{
			QObject::connect(bubble_sound_timer[1], &QTimer::timeout, [&]() {
				bubble_player[1]->stop();
				bubble_player[1]->play();
				// loop time
				bubble_sound_timer[1]->start(bubble_player[1]->duration());
			});
			bubble_sound_timer[1]->start(bubble_player[1]->duration() / 2);
		}
	});


	//
	/*for (int i = 0; i < 2; i++) {
		propeller_sound_timer[i] = new QTimer;
	}*/

	// bubble sound 
	//QObject::connect(propeller_player[0], &QMediaPlayer::mediaStatusChanged,
	//	[&](QMediaPlayer::MediaStatus status) {
	//	if (status == QMediaPlayer::LoadedMedia) //when loading is finished
	//	{
	//		QObject::connect(propeller_sound_timer[0], &QTimer::timeout, [&]() {
	//			propeller_player[0]->stop();
	//			propeller_player[0]->play();
	//			// loop time
	//			propeller_sound_timer[0]->start(propeller_player[0]->duration());
	//		});
	//		propeller_sound_timer[0]->start();
	//	}
	//});
	//QObject::connect(propeller_player[1], &QMediaPlayer::mediaStatusChanged,
	//	[&](QMediaPlayer::MediaStatus status) {
	//	if (status == QMediaPlayer::LoadedMedia) //when loading is finished
	//	{
	//		QObject::connect(propeller_sound_timer[1], &QTimer::timeout, [&]() {
	//			propeller_player[1]->stop();
	//			propeller_player[1]->play();
	//			// loop time
	//			propeller_sound_timer[1]->start(propeller_player[1]->duration());
	//		});
	//		propeller_sound_timer[1]->start(propeller_player[1]->duration() / 2);
	//	}
	//});

}

void MainOGLWidget::stopBackgroundSound(void)
{
	bubble_sound_timer[0]->stop();
	bubble_sound_timer[1]->stop();
}


// SLOTS
void MainOGLWidget::PhysicsEngine_Func(void) 
{

	// ROV
	// Physics engine

	// update physics quantities
	rov->calculateAveragePhysicsCenter();
	rov->calculateAverageInertia();

	// hydrostatic
	// add gravity
	rov->addHydroStaticForce();

	// hydrodynamic
	// add propeller force
	rov->clearMotionPower();
	if (mode_manu) {
		for (int i = 0; i < NumMotion; i++) {
			rov->motionPower[i] = this->menu_motionPower[i];
		}
	}
	rov->runPID(global_target_position, target_orientation, target_velocity, target_angular_velocity, PhisicsEngineTimeInterval);
	rov->calculatePropellerVector();
	rov->addPropellerForce();

	// add Drag
	glm::vec3 drag = rov->addDragForce();
	//this->ui.GlobalForceBrowser->setText(QString::number(rov->PE.force.x, 'f', 4) + ",  " + QString::number(rov->PE.force.y, 'f', 4) + ",  " + QString::number(rov->PE.force.z, 'f', 4) + "  ");
	//this->ui.DragForceBrowser->setText(QString::number(drag.x, 'f', 4) + ",  " + QString::number(drag.y, 'f', 4) + ",  " + QString::number(drag.z, 'f', 4) + "  ");

	// run
	rov->PE.Run(PhisicsEngineTimeInterval);
	// update Model Matrix 
	rov->calculateTransferMatrix();


	// collisioon detection
	// if its true, undo this frame motion
	// scene collision
	bool collision = false;
	glm::vec3 collision_point_in_world(0.0), collision_point_in_body(0.0), accerlation, collision_reaction_force;
	if (terrain_scene_loaded) {
		collision_state = seabed->collision(rov->total_displacement, rov->modelMatrix, collision_point_in_world);
		if (collision_state == Contact) {
			/*rov->PE.unMovePositiveY();
			rov->PE.unMovePositiveY();

			accerlation = seabed->getElasticity() * glm::vec3(-rov->PE.v.x, fabs(rov->PE.v.y), -rov->PE.v.z) / PhisicsEngineTimeInterval;
			collision_reaction_force = ((float)rov->PE.mass) * accerlation;

			rov->PE.addGlobalForce(collision_reaction_force, glm::vec3(0.0), rov->RotateMatrixInv);*/
			collision = true;
		}
		if (old_collision_state == Safe && collision_state != Safe) {
			emit warning(true);
		}
		else if(old_collision_state != Safe && collision_state == Safe){
			emit warning(false);
		}
		old_collision_state = collision_state;
	}

	// oilplatform collision
	if(oilplatform->intersect(rov->aabb))
	{
		/*this->rov->PE.unMove(1.0);
		accerlation = 1.6f * glm::vec3(-rov->PE.v.x, -rov->PE.v.y, -rov->PE.v.x) / PhisicsEngineTimeInterval;
		collision_reaction_force = ((float)rov->PE.mass) * accerlation;
		rov->PE.addGlobalForce(collision_reaction_force, glm::vec3(0.0), rov->RotateMatrixInv);*/
		collision = true;
	}

	// seaplane collision
	if (rov->total_displacement.y > 0.0) {
		// magic number 2.45
		/*accerlation = 2.45f * glm::vec3(0.0, -fabs(rov->PE.v.y), 0.0) / PhisicsEngineTimeInterval;
		collision_reaction_force = ((float)rov->PE.mass) * accerlation;

		rov->PE.addGlobalForce(collision_reaction_force, glm::vec3(0.0), rov->RotateMatrixInv);*/
		collision = true;
	}

	if (collision) {
		rov->undoModelMatrix();
		accerlation = -2.0f * glm::vec3(rov->PE.v.x, rov->PE.v.y, rov->PE.v.z) / PhisicsEngineTimeInterval;
		collision_reaction_force = ((float)rov->PE.mass) * accerlation;

		//rov->addHydroStaticForce();
		rov->PE.addGlobalForce(collision_reaction_force, this->rov->mass_center_Average, rov->RotateMatrixInv);
		//glm::vec3 drag = rov->addDragForce();
		rov->PE.Run(PhisicsEngineTimeInterval);
		// update Model Matrix 
		rov->calculateTransferMatrix();
	}
}

void MainOGLWidget::setInformation(void) 
{
	// Left side window
	glm::vec3 Position = rov->total_displacement;
	glm::vec3 Motion_Velocity = glm::mat3(rov->RotateMatrixInv) * rov->PE.v;
	//glm::vec3 MassCenterInWorld = rov->modelMatrix * rov->mass_center;
	//glm::vec3 BouyancyInWorld = rov->modelMatrix * rov->volume_center;
	glm::vec3 RotateAngle = glm::degrees(rov->total_rotate_radian);
	//RotateAngle = glm::vec3(fmod(RotateAngle.x, 360), fmod(RotateAngle.y, 360), fmod(RotateAngle.z, 360));
	glm::vec3 AngularVelocity = glm::degrees(rov->PE.omega);

	// Position Information
	this->ui.Position_X_LCD->display(Position.x);
	this->ui.Position_Y_LCD->display(Position.y);
	this->ui.Position_Z_LCD->display(Position.z);

	// Orientation Information
	this->ui.Orientation_Pitch_LCD->display(RotateAngle.x);
	this->ui.Orientation_Yaw_LCD->display(RotateAngle.y);
	this->ui.Orientation_Roll_LCD->display(RotateAngle.z);

	// Velocity Information
	this->ui.Velocity_Sway_LCD->display(Motion_Velocity.x);
	this->ui.Velocity_Heave_LCD->display(Motion_Velocity.y);
	this->ui.Velocity_Surge_LCD->display(Motion_Velocity.z);

	// Angular Velocity Information
	this->ui.AngularVelocity_Pitch_LCD->display(AngularVelocity.x);
	this->ui.AngularVelocity_Yaw_LCD->display(-AngularVelocity.y);
	this->ui.AngularVelocity_Roll_LCD->display(AngularVelocity.z);
			 

	// Right side window
	// Propeller Power
	if (this->ui.PowerButton->isChecked()) {
		this->ui.PFLCD->display(rov->propeller[PF].getPower(rov->motionVector.getVectorComponent(PF)));
		this->ui.PVLCD->display(rov->propeller[PV].getPower(rov->motionVector.getVectorComponent(PV)));
		this->ui.PALCD->display(rov->propeller[PA].getPower(rov->motionVector.getVectorComponent(PA)));
		this->ui.SFLCD->display(rov->propeller[SF].getPower(rov->motionVector.getVectorComponent(SF)));
		this->ui.SVLCD->display(rov->propeller[SV].getPower(rov->motionVector.getVectorComponent(SV)));
		this->ui.SALCD->display(rov->propeller[SA].getPower(rov->motionVector.getVectorComponent(SA)));
	}
	else if (this->ui.RPMButton->isChecked()) {
		this->ui.PFLCD->display(rov->propeller[PF].getRPM(rov->motionVector.getVectorComponent(PF)));
		this->ui.PVLCD->display(rov->propeller[PV].getRPM(rov->motionVector.getVectorComponent(PV)));
		this->ui.PALCD->display(rov->propeller[PA].getRPM(rov->motionVector.getVectorComponent(PA)));
		this->ui.SFLCD->display(rov->propeller[SF].getRPM(rov->motionVector.getVectorComponent(SF)));
		this->ui.SVLCD->display(rov->propeller[SV].getRPM(rov->motionVector.getVectorComponent(SV)));
		this->ui.SALCD->display(rov->propeller[SA].getRPM(rov->motionVector.getVectorComponent(SA)));
	}
	else if (this->ui.ForceButton->isChecked()) {
		this->ui.PFLCD->display(rov->propeller[PF].getForce(rov->motionVector.getVectorComponent(PF)));
		this->ui.PVLCD->display(rov->propeller[PV].getForce(rov->motionVector.getVectorComponent(PV)));
		this->ui.PALCD->display(rov->propeller[PA].getForce(rov->motionVector.getVectorComponent(PA)));
		this->ui.SFLCD->display(rov->propeller[SF].getForce(rov->motionVector.getVectorComponent(SF)));
		this->ui.SVLCD->display(rov->propeller[SV].getForce(rov->motionVector.getVectorComponent(SV)));
		this->ui.SALCD->display(rov->propeller[SA].getForce(rov->motionVector.getVectorComponent(SA)));
	}
}

void MainOGLWidget::recordPIDInformation(void)
{
	// Left side window
	glm::vec3 Position = rov->total_displacement;
	glm::vec3 Motion_Velocity = glm::mat3(rov->RotateMatrixInv) * rov->PE.v;
	//glm::vec3 MassCenterInWorld = rov->modelMatrix * rov->mass_center;
	//glm::vec3 BouyancyInWorld = rov->modelMatrix * rov->volume_center;
	glm::vec3 RotateAngle = glm::degrees(rov->total_rotate_radian);
	RotateAngle = glm::vec3(fmod(RotateAngle.x, 360), fmod(RotateAngle.y, 360), fmod(RotateAngle.z, 360));
	glm::vec3 AngularVelocity = glm::degrees(rov->PE.omega);

	// Position PID record
	this->ui.Position_X_PIDWidget->record(Position.x);
	this->ui.Position_Y_PIDWidget->record(Position.y);
	this->ui.Position_Z_PIDWidget->record(Position.z);

	// RotateAngle PID record
	this->ui.Orientation_Pitch_PIDWidget->record(RotateAngle.x);
	this->ui.Orientation_Yaw_PIDWidget->record(RotateAngle.y);
	this->ui.Orientation_Roll_PIDWidget->record(RotateAngle.z);

	// Velocity PID record
	this->ui.Velocity_Sway_PIDWidget->record(Motion_Velocity.x);
	this->ui.Velocity_Heave_PIDWidget->record(Motion_Velocity.y);
	this->ui.Velocity_Surge_PIDWidget->record(Motion_Velocity.z);

	// Angular Velocity PID record
	this->ui.AngularVelocity_Pitch_PIDWidget->record(AngularVelocity.x);
	this->ui.AngularVelocity_Yaw_PIDWidget->record(-AngularVelocity.y);
	this->ui.AngularVelocity_Roll_PIDWidget->record(AngularVelocity.z);

	glm::vec3 position_error(0.0);
	if (this->rov->position_PID_worked[X]) {
		position_error.x = global_target_position.x - Position.x;
	}
	if (this->rov->position_PID_worked[Y]) {
		position_error.y = global_target_position.y - Position.y;
	}
	if (this->rov->position_PID_worked[Z]) {
		position_error.z = global_target_position.z - Position.z;
	}
	glm::vec3 orientation_error(0.0);
	if (this->rov->orientation_PID_worked[X]) {
		orientation_error.x = target_orientation.x - RotateAngle.x;
	}
	if (this->rov->orientation_PID_worked[Y]) {
		orientation_error.y = target_orientation.y - RotateAngle.y;
	}
	if (this->rov->orientation_PID_worked[Z]) {
		orientation_error.z = target_orientation.z - RotateAngle.z;
	}
	this->infocouplingWidget->record(position_error, orientation_error);
}

void MainOGLWidget::changeHeaveValue(int value)
{
	menu_motionPower[Heave] = -value / 50.0;
}

void MainOGLWidget::changeSurgeValue(int value)
{
	menu_motionPower[Surge] = value / 50.0;
}

void MainOGLWidget::changeSwayValue(int value)
{
	menu_motionPower[Sway] = -value / 50.0;
}

void MainOGLWidget::changeYawValue(int value)
{
	menu_motionPower[Yaw] = value / 50.0;
}

void MainOGLWidget::changeRollValue(int value)
{
	menu_motionPower[Roll] = value / 50.0;
}

void MainOGLWidget::changePitchValue(int value)
{
	menu_motionPower[Pitch] = -value / 1000.0;
}

void MainOGLWidget::change2JoystickMode(bool check)
{
	mode_joystick = check;
	mode_manu = !check;
	// close manu mode ui
	this->ui.setPositionTarget_PB->setEnabled(false);
	this->ui.setOrientationTarget_PB->setEnabled(false);
	this->ui.position_X_Input->setReadOnly(true);
	this->ui.position_Y_Input->setReadOnly(true);
	this->ui.position_Z_Input->setReadOnly(true);
	//this->ui.orientation_Pitch_Input->setReadOnly(false);
	this->ui.orientation_Yaw_Input->setReadOnly(true);
	this->ui.orientation_Roll_Input->setReadOnly(true);
	// clear manu mode PID setting
	this->rov->position_PID_worked[X] = this->rov->position_PID_worked[Y] = this->rov->position_PID_worked[Z] = false;
	this->rov->orientation_PID_worked[X] = this->rov->orientation_PID_worked[Y] = this->rov->orientation_PID_worked[Z] = false;
	this->rov->velocity_PID_worked[X] = this->rov->velocity_PID_worked[Y] = this->rov->velocity_PID_worked[Z] = false;
	this->rov->angularVelocity_PID_worked[X] = this->rov->angularVelocity_PID_worked[Y] = this->rov->angularVelocity_PID_worked[Z] = false;
	global_target_position = glm::vec3(0.0f);
	target_orientation = glm::vec3(0.0f);

	// start joystick
	mainROVControlJoystickTimer->start(10);
}

void MainOGLWidget::change2ManuMode(bool check)
{
	mode_manu = check;
	 mode_joystick = !check;
	// close manu mode ui
	this->ui.setPositionTarget_PB->setEnabled(true);
	this->ui.setOrientationTarget_PB->setEnabled(true);
	this->ui.position_X_Input->setReadOnly(false);
	this->ui.position_Y_Input->setReadOnly(false);
	this->ui.position_Z_Input->setReadOnly(false);
	//this->ui.orientation_Pitch_Input->setReadOnly(false);
	this->ui.orientation_Yaw_Input->setReadOnly(false);
	this->ui.orientation_Roll_Input->setReadOnly(false);
	// clear manu mode PID setting
	this->rov->position_PID_worked[X] = this->rov->position_PID_worked[Y] = this->rov->position_PID_worked[Z] = false;
	this->rov->orientation_PID_worked[X] = this->rov->orientation_PID_worked[Y] = this->rov->orientation_PID_worked[Z] = false;
	this->rov->velocity_PID_worked[X] = this->rov->velocity_PID_worked[Y] = this->rov->velocity_PID_worked[Z] = false;
	this->rov->angularVelocity_PID_worked[X] = this->rov->angularVelocity_PID_worked[Y] = this->rov->angularVelocity_PID_worked[Z] = false;
	global_target_position = glm::vec3(0.0f);
	target_orientation = glm::vec3(0.0f);

	// stop joystick
	mainROVControlJoystickTimer->stop();
}

void MainOGLWidget::switch_controlling_robotarm(bool on_off)
{
	if (on_off) {
		robotArmControlJoystickTimer->start(10);
	}
	else {
		robotArmControlJoystickTimer->stop();
	}
}

void MainOGLWidget::switch_displaying_boundingvolume(bool on_off)
{
	showboundingbox = on_off;
}

void MainOGLWidget::switch_recording_pathline(bool on_off)
{
	if (on_off) {
		showpathline = true;
		pathlineTimer->start(100);
	}
	else {
		showpathline = false;
		position_list.clear();
		pathlineTimer->stop();
	}
}

void MainOGLWidget::on_PowerButton_clicked(bool checked)
{
	this->ui.RPMButton->setChecked(!checked);
	this->ui.ForceButton->setChecked(!checked);
}

void MainOGLWidget::on_RPMButton_clicked(bool checked)
{
	this->ui.PowerButton->setChecked(!checked);
	this->ui.ForceButton->setChecked(!checked);
}

void MainOGLWidget::on_ForceButton_clicked(bool checked)
{
	this->ui.PowerButton->setChecked(!checked);
	this->ui.RPMButton->setChecked(!checked);
}

void MainOGLWidget::changeCamera(QAction * action)
{
	if (change_camera_pos.x < dividerPosition && change_camera_pos.y < this->height() - screenViewport[0][1]) {
		screenCameraID[0] = action->data().toInt();
	}
	else if (change_camera_pos.y < screenViewport[3][1]) {
		screenCameraID[1] = action->data().toInt();
	}
	else if (change_camera_pos.y < screenViewport[2][1]) {
		screenCameraID[2] = action->data().toInt();
	}
	else if (change_camera_pos.y < screenViewport[1][1]) {
		screenCameraID[3] = action->data().toInt();
	}
	else {
		screenCameraID[4] = action->data().toInt();
	}
	if (action->data().toInt() == total_camera_amount - 1) {
		glm::vec3 position = rov->total_displacement + glm::vec3(0.0, 2.0, 5.0);
		camera[total_camera_amount - 1].setPositionPoint(position.x, position.y, position.z);
		camera[total_camera_amount - 1].setFrontVector(0.0, 0.0, -1.0);
		camera[total_camera_amount - 1].setViewUpVector(0.0, 1.0, 0.0);
		camera[total_camera_amount - 1].setRightVector(1.0, 0.0, 0.0);
	}
}

void MainOGLWidget::on_setPositionTarget_PB_clicked(void)
{
	infocouplingWidget->show();
	this->rov->position_PID_worked[X] = this->rov->position_PID_worked[Y] = this->rov->position_PID_worked[Z] = false;
	global_target_position = glm::vec3(0.0f);
	if (this->ui.position_X_Input->text() != QString("")) {
		global_target_position.x = this->ui.position_X_Input->text().toDouble();
		this->ui.Position_X_PIDWidget->setTarget(global_target_position.x);
		this->rov->position_PID_worked[X] = true;
	}
	if (this->ui.position_Y_Input->text() != QString("")) {
		global_target_position.y = this->ui.position_Y_Input->text().toDouble();
		this->ui.Position_Y_PIDWidget->setTarget(global_target_position.y);
		this->rov->position_PID_worked[Y] = true;
	}
	if (this->ui.position_Z_Input->text() != QString("")) {
		global_target_position.z = this->ui.position_Z_Input->text().toDouble();
		this->ui.Position_Z_PIDWidget->setTarget(global_target_position.z);
		this->rov->position_PID_worked[Z] = true;
	}
}

void MainOGLWidget::on_setOrientationTarget_PB_clicked()
{
	infocouplingWidget->show();
	this->rov->orientation_PID_worked[X] = this->rov->orientation_PID_worked[Y] = this->rov->orientation_PID_worked[Z] = false;
	target_orientation = glm::vec3(0.0f);
	if (this->ui.orientation_Pitch_Input->text() != QString("")) {
		target_orientation.x = this->ui.orientation_Pitch_Input->text().toDouble();
		this->ui.Orientation_Pitch_PIDWidget->setTarget(target_orientation.x);
		this->rov->orientation_PID_worked[X] = true;
	}
	if (this->ui.orientation_Yaw_Input->text() != QString("")) {
		target_orientation.y = this->ui.orientation_Yaw_Input->text().toDouble();
		this->ui.Orientation_Yaw_PIDWidget->setTarget(target_orientation.y);
		this->rov->orientation_PID_worked[Y] = true;
	}
	if (this->ui.orientation_Roll_Input->text() != QString("")) {
		target_orientation.z = this->ui.orientation_Roll_Input->text().toDouble();
		this->ui.Orientation_Roll_PIDWidget->setTarget(target_orientation.z);
		this->rov->orientation_PID_worked[Z] = true;
	}
}

void MainOGLWidget::processMainROVControlJoystick(void)
{
	JOYCAPS main_rov_control_jc;;
	unsigned int numDevs, uiXmax, uiYmax, uiZmax, uiRmax;

	numDevs = joyGetNumDevs();
	//cout << "num Devs:" << numDevs << endl;
	if (numDevs == 0) {
		return;
	}
	if (JOYERR_NOERROR != joyGetDevCaps(JOYSTICKID1, &main_rov_control_jc, sizeof(main_rov_control_jc))) //判斷ID1手把是否可以使用
	{
		cout << "There is no joystick can be use for control the main rov." << endl;
		return;
	}
	else 
	{
		uiXmax = main_rov_control_jc.wXmax;
		uiYmax = main_rov_control_jc.wYmax;
		uiZmax = main_rov_control_jc.wZmax;
		uiRmax = main_rov_control_jc.wRmax;

		joystick_info[Joystick1]->dwSize = sizeof(joyinfoex_tag);
		joystick_info[Joystick1]->dwFlags = JOY_RETURNALL;
		MMRESULT joy_error = joyGetPosEx(JOYSTICKID1, joystick_info[Joystick1]);
		if (joy_error != JOYERR_NOERROR) {
			cout << "Main ROV control joystick has error: ";
			if (joy_error == MMSYSERR_NODRIVER) {
				cout << "MMSYSERR_NODRIVER" << endl;
			}
			else if (joy_error == MMSYSERR_INVALPARAM) {
				cout << "MMSYSERR_INVALPARAM" << endl;
			}
			else if (joy_error == MMSYSERR_BADDEVICEID) {
				cout << "MMSYSERR_BADDEVICEID" << endl;
			}
			else if (joy_error == JOYERR_UNPLUGGED) {
				cout << "JOYERR_UNPLUGGED" << endl;
			}
			else if (joy_error == JOYERR_PARMS) {
				cout << "JOYERR_PARMS" << endl;
			}
		}else
		{
			/*printf("Joystick 1:\n");
			printf("dwXpos: 0x%09d\n", joystick_1->dwXpos);
			printf("dwYpos: 0x%09d\n", joystick_1->dwYpos);
			printf("dwZpos: 0x%09d\n", joystick_1->dwZpos);
			printf("dwPOV: 0x%09d\n", joystick_1->dwPOV);
			printf("dwButtons: 0x%09d\n", joystick_1->dwButtons);*/
			// normalize main handle bar
			float sway = ((joystick_info[Joystick1]->dwXpos * 2.0) - uiXmax) / uiXmax;
			float surge = -((joystick_info[Joystick1]->dwYpos * 2.0) - uiYmax) / uiYmax;
			float heave = -((joystick_info[Joystick1]->dwZpos * 2.0) - uiZmax) / uiZmax;
			float yaw = ((joystick_info[Joystick1]->dwRpos * 2.0) - uiRmax) / uiRmax;

			// catch 8 axis button
			int axis_8 = -1;
			if (joystick_info[Joystick1]->dwPOV != 65535) {
				axis_8 = joystick_info[Joystick1]->dwPOV / 4500;
			}


			/* use joystick value */

			// velocity PID
			target_velocity = glm::vec3(0.0);
			target_velocity.x = -sway * SwayMaxSpeed;
			this->ui.Velocity_Sway_PIDWidget->setTarget(target_velocity.x);
			this->rov->velocity_PID_worked[X] = true;

			if (!auto_depth) {
				target_velocity.y = -heave * HeaveMaxSpeed;
				this->ui.Velocity_Heave_PIDWidget->setTarget(target_velocity.y);
			}
			this->rov->velocity_PID_worked[Y] = !auto_depth;
			this->rov->position_PID_worked[Y] = auto_depth;

			target_velocity.z = surge * SurgeMaxSpeed;
			this->ui.Velocity_Surge_PIDWidget->setTarget(target_velocity.z);
			this->rov->velocity_PID_worked[Z] = true;

			// angular velocity PID
			target_angular_velocity = glm::vec3(0.0);
			if (!auto_heading)
			{
				target_angular_velocity.y = -yaw * YawMaxRadianSpeed;
				glm::vec3 target_angular_velocity_angle = glm::degrees(target_angular_velocity);
				this->ui.AngularVelocity_Yaw_PIDWidget->setTarget(-target_angular_velocity_angle.y);
			}
			this->rov->angularVelocity_PID_worked[Y] = !auto_heading;
			this->rov->orientation_PID_worked[Y] = auto_heading;

			//this->ui.AngularVelocity_Roll_PIDWidget->setTarget(target_angular_velocity_angle.z);
			//this->rov->angularVelocity_PID_worked[Z] = true;


			// 8 axis event
			// 
			if (axis_8 == 0) {
				camera[0].zoom(ZOOMIN);
			}
			if (axis_8 == 4) {
				camera[0].zoom(ZOOMOUT);
			}

			vector<int> press_button_array;
			vector<int> release_button_array;
			decodingJoyButton(Joystick1, &press_button_array, &release_button_array);
			for (int i = 0; i < press_button_array.size(); i++) {
				processPressButton(press_button_array[i], JOYSTICKID1);
			}
			for (int i = 0; i < release_button_array.size(); i++) {
				processReleaseButton(release_button_array[i], JOYSTICKID1);
			}
		}
	}
}

void MainOGLWidget::processRobotArmControlJoystick(void)
{
	JOYCAPS robot_arm_control_jc;;
	unsigned int numDevs, uiXmax, uiYmax, uiZmax, uiRmax;

	numDevs = joyGetNumDevs();
	if (numDevs == 0) {
		return;
	}

	if (JOYERR_NOERROR != joyGetDevCaps(JOYSTICKID2, &robot_arm_control_jc, sizeof(robot_arm_control_jc))) //判斷ID2手把是否可以使用
	{
		cout << "There is no joystick can be use for control the robot arm." << endl;
		return;
	}
	else
	{
		joystick_info[Joystick2]->dwSize = sizeof(joyinfoex_tag);
		joystick_info[Joystick2]->dwFlags = JOY_RETURNALL;
		MMRESULT joy_error = joyGetPosEx(JOYSTICKID2, joystick_info[Joystick2]);
		if (joy_error != JOYERR_NOERROR) {
			cout << "Robot arm control joystick has error: ";
			if (joy_error == MMSYSERR_NODRIVER) {
				cout << "MMSYSERR_NODRIVER" << endl;
			}
			else if (joy_error == MMSYSERR_INVALPARAM) {
				cout << "MMSYSERR_INVALPARAM" << endl;
			}
			else if (joy_error == MMSYSERR_BADDEVICEID) {
				cout << "MMSYSERR_BADDEVICEID" << endl;
			}
			else if (joy_error == JOYERR_UNPLUGGED) {
				cout << "JOYERR_UNPLUGGED" << endl;
			}
			else if (joy_error == JOYERR_PARMS) {
				cout << "JOYERR_PARMS" << endl;
			}
		}
		else
		{
			uiXmax = robot_arm_control_jc.wXmax;
			uiYmax = robot_arm_control_jc.wYmax;
			uiZmax = robot_arm_control_jc.wZmax;
			uiRmax = robot_arm_control_jc.wRmax;

#ifdef _DEBUG
#define _DEBUG
			/*system("cls");
			cout << "Robot arm control joystick signal: " << endl;
			cout << "dwXpos: " << joystick_info[Joystick2]->dwXpos << endl;
			cout << "dwYpos: " << joystick_info[Joystick2]->dwYpos << endl;
			cout << "dwZpos: " << joystick_info[Joystick2]->dwZpos << endl;
			cout << "dwRpos: " << joystick_info[Joystick2]->dwRpos << endl;
			cout << "dwButtons:" << joystick_info[Joystick2]->dwButtons << endl;*/
#endif

			// robot arm use
			float arm_1_omega = ((joystick_info[Joystick2]->dwXpos * 2.0) - uiXmax) / uiXmax;
			float arm_2_omega = -((joystick_info[Joystick2]->dwYpos * 2.0) - uiYmax) / uiYmax;
			float control_joint_1_omega = ((joystick_info[Joystick2]->dwRpos * 2.0) - uiRmax) / uiRmax;
			this->rov->controlRobotArm(arm_1_omega, Arm1);
			this->rov->controlRobotArm(arm_2_omega, Arm2);
			this->rov->controlRobotArm(-control_joint_1_omega, ControlJoint1);


			// paw use button to control
			vector<int> press_button_array;
			vector<int> release_button_array;
			decodingJoyButton(Joystick2, &press_button_array, &release_button_array);
			for (int i = 0; i < press_button_array.size(); i++) {
				processPressButton(press_button_array[i], JOYSTICKID2);
			}
			for (int i = 0; i < release_button_array.size(); i++) {
				processReleaseButton(release_button_array[i], JOYSTICKID2);
			}
		}
	}

}

void MainOGLWidget::decodingJoyButton(int num_of_joystick, vector<int>* press_button_array, vector<int>* release_button_array)
{
	// decoding parameter
	int bit = 32768;
	int buttonID = 15;


	// catch button
	press_button_array->clear();
	release_button_array->clear();
	int new_button_number = joystick_info[num_of_joystick]->dwButtons;
	int new_button_amount = joystick_info[num_of_joystick]->dwButtonNumber;
	if (new_button_amount > 0 || old_button_amount > 0) {
		int count = new_button_amount;
		while (count > 0) {
			if (new_button_number & bit) {
				press_button_array->push_back(buttonID);
				count--;
			}
			bit = bit >> 1;
			buttonID -= 1;
		}
		int release_button_number = old_button_number[num_of_joystick] - new_button_number;
		count = old_button_amount[num_of_joystick] - new_button_amount;
		while (count > 0) {
			if (release_button_number & bit) {
				release_button_array->push_back(buttonID);
				count--;
			}
			bit = bit >> 1;
			buttonID -= 1;
		}
		old_button_number[num_of_joystick] = new_button_number;
	}
	old_button_amount[num_of_joystick] = new_button_amount;
}

void MainOGLWidget::processPressButton(int buttonID, int joy_number)
{
	if (joy_number == JOYSTICKID1) {
		switch (buttonID) {
		default:
			break;
		}
	}
	if (joy_number == JOYSTICKID2) {
		switch (buttonID) {
		case 0:
			this->rov->controlRobotArm(1.0f, Paw1);
			this->rov->controlRobotArm(-1.0f, Paw2);
			break;
		case 1:
			this->rov->controlRobotArm(-1.0f, Paw1);
			this->rov->controlRobotArm(1.0f, Paw2);
			break;
		case 2:
			this->rov->controlRobotArm(-1.0f, ControlJoint2);
			break;
		case 3:
			this->rov->controlRobotArm(1.0f, ControlJoint2);
			break;
		default:
			break;
		}
	}
}

void MainOGLWidget::processReleaseButton(int buttonID, int joy_number)
{
	if (joy_number == JOYSTICKID1) {
		switch (buttonID) {
		case 4:
			auto_depth = !auto_depth;
			if (auto_depth) {
				global_target_position.y = rov->total_displacement.y;
			}
			else {
				global_target_position.y = 0;
			}
			this->ui.Position_Y_PIDWidget->setTarget(global_target_position.y);
			break;
		case 9:
			stable_attitude = !stable_attitude;
			if (stable_attitude) {
				target_orientation.x = glm::degrees(0.0f);
				target_orientation.z = glm::degrees(0.0f);
			}
			else {
				target_orientation.x = 0.0;
				target_orientation.z = 0.0;
			}
			this->ui.Orientation_Pitch_PIDWidget->setTarget(target_orientation.x);
			this->ui.Orientation_Roll_PIDWidget->setTarget(target_orientation.z);
			this->rov->orientation_PID_worked[X] = stable_attitude;
			this->rov->orientation_PID_worked[Z] = stable_attitude;
			break;
		case 10:
			auto_heading = !auto_heading;
			if (auto_heading) {
				target_orientation.y = glm::degrees(rov->total_rotate_radian.y);
			}
			else {
				target_orientation.y = 0.0;
			}
			this->ui.Orientation_Yaw_PIDWidget->setTarget(target_orientation.y);
			break;
		default:
			break;
		}
	}
	if (joy_number == JOYSTICKID2) {
		switch (buttonID) {
		default:
			break;
		}
	}
}

void MainOGLWidget::playWarning(bool flag)
{
	if (flag) {
		// warning sound 
		QObject::connect(warning_sound_timer[0], &QTimer::timeout, [&]() {
			warning_player[0]->stop();
			warning_player[0]->play();
			// loop time
			warning_sound_timer[0]->start(warning_player[0]->duration());
		});
		warning_sound_timer[0]->start();


		QObject::connect(warning_sound_timer[1], &QTimer::timeout, [&]() {
			warning_player[1]->stop();
			warning_player[1]->play();
			// loop time
			warning_sound_timer[1]->start(warning_player[1]->duration());
		});
		warning_sound_timer[1]->start(warning_player[1]->duration() / 2);

	}
	else {
		warning_player[0]->stop();
		warning_sound_timer[0]->stop();
		warning_player[1]->stop();
		warning_sound_timer[1]->stop();
	}
}

void MainOGLWidget::updateWaterTime(void)
{
	this->water_time += 0.01;
}

void MainOGLWidget::recordPathPosition(void)
{
	position_list.push(rov->total_displacement);
}

void MainOGLWidget::myUpdate()
{
	update();
	//float fps = 0;
	//static int fuck = 0;
	//static chrono::steady_clock::time_point last_point;
	//if (fuck == 0) {
	//	last_point = chrono::steady_clock::now();
	//	fuck += 1;
	//}
	//else {
	//	fps = 1000.0 / (chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - last_point)).count();
	//	last_point = chrono::steady_clock::now();
	//}
	////system("cls");
	//cout << "fps: " << fps << endl;
}
