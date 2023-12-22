

#include "SceneModel.h"
#include <math.h>

// three local variables with the hardcoded file names
const char* groundModelName		= "./models/randomland.dem";
const char* characterModelName	= "./models/human_lowpoly_100.obj";
const char* motionBvhStand		= "./models/stand.bvh";
const char* motionBvhRun		= "./models/fast_run.bvh";
const char* motionBvhveerLeft	= "./models/veer_left.bvh";
const char* motionBvhveerRight	= "./models/veer_right.bvh";
const float cameraSpeed = 300.0; 
const float playerSpeed = 0.0f; // Player speed for movement

const Homogeneous4 sunDirection(0.5, -0.5, 0.3, 1.0);
const GLfloat groundColour[4] = { 0.3, 0.5, 0.2, 1.0 };
const GLfloat boneColour[4] = { 0.5, 0.2, 0.6, 1.0 };
const GLfloat playerColour[4] = { 1.0, 1.0, 1.0, 1.0 };
const GLfloat sunAmbient[4] = {0.1, 0.1, 0.1, 1.0 };
const GLfloat sunDiffuse[4] = {0.7, 0.7, 0.7, 1.0 };
const GLfloat blackColour[4] = {0.0, 0.0, 0.0, 1.0};

static auto startTime = std::chrono::high_resolution_clock::now();

// constructor
SceneModel::SceneModel()
	{ // constructor
	// load the object models from files
	groundModel.ReadFileTerrainData(groundModelName, 10);

	// load the animation data from files
	restPose.ReadFileBVH(motionBvhStand);
	runCycle.ReadFileBVH(motionBvhRun);
	veerLeftCycle.ReadFileBVH(motionBvhveerLeft);
	veerRightCycle.ReadFileBVH(motionBvhveerRight);
	playerController.ReadFileBVH(motionBvhRun);

	// set the world to opengl matrix
	world2OpenGLMatrix = Matrix4::RotateX(90.0); // ccw rotation 
	CameraTranslateMatrix = Matrix4::Translate(Cartesian3(-5, 15, -15.5));
	CameraRotationMatrix = Matrix4::RotateX(90.0) * Matrix4::RotateZ(30.0);

	// Set the camera position, direction and up
	m_camera = new Camera(Cartesian3(-5.0f, 15.0f, 10.0f), Cartesian3(0.0f, 0.0f, 1.0f), Cartesian3(0.0f, 1.0f, 0.0f));

	// initialize the character's position and rotation
	EventCharacterReset();

	// and set the frame number to 0
	frameNumber = 0;

	// Set the character start position and direction
	m_characterPosition = Cartesian3(0.0f, 0.0f, -10.0f);
	m_characterDirection = Cartesian3(0.0f, 0.0f, 1.0f);

	// Construct the player look marix using current character position and direction
	m_playerLookMatrix = Matrix4::Look(m_characterPosition, m_characterPosition + m_characterDirection, Cartesian3(0.0f, 1.0f, 0.0f));

	// Set the starting position of the character which is constantly running using the run animation
	m_controllerLessRunCyclePosition = Cartesian3(30.0f, 0.0f, -10.0f);

	// Set the current animation state of the player to be Idle to begin with
	} // constructor


// Destructor
SceneModel::~SceneModel()
{
	// safely release heap allocated memory when we exit application
	delete m_camera;
}

// routine that updates the scene for the next frame
void SceneModel::Update()
	{ // Update()
	
	// get the height of the floor at the point the character is currently at
	//auto floor = groundModel.getHeight(m_characterPosition.x, m_characterPosition.z);
	m_characterPosition.y = 0.0f; // set y to the floor height for the character to make it run on the terrain instead of through

	// Construct and update the look matrix for the character so he orients and moves correctly in the scene
	// when the user makes changes
	m_playerLookMatrix = Matrix4::Look(m_characterPosition, m_characterPosition + m_characterDirection, Cartesian3(0.0f, 1.0f, 0.0f));

	// Get the height of the terrain for the position of the run cycle animation loop character
	auto runCycleFloor = groundModel.getHeight(m_controllerLessRunCyclePosition.x, m_controllerLessRunCyclePosition.z);
	m_controllerLessRunCyclePosition.y = runCycleFloor;
	// For the runnign animation cycle we are translating it across the terrain
	// if it goes close to the boundary of the edge, we reset position
	if(m_controllerLessRunCyclePosition.z < 130)
	{	
		// m_controllerLessRunCyclePosition.z += 1.0f;
	} else
	{
		m_controllerLessRunCyclePosition.z = -20.0f;
	}

	if(playerController.m_AnimState == Running)
	{
		auto forward = Cartesian3(m_playerLookMatrix[0][2], m_playerLookMatrix[1][2], m_playerLookMatrix[2][2]);
		m_characterPosition = m_characterPosition + forward * playerSpeed;
	}
	// Update the camera 
	m_camera->Update();

	// increment the frame counter
	frameNumber++;
	
	} // Update()


// routine to tell the scene to render itself
void SceneModel::Render()
	{ // Render()
	// enable Z-buffering
	glEnable(GL_DEPTH_TEST);
	
	// set lighting parameters
	glShadeModel(GL_FLAT);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_AMBIENT, sunAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, sunDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, blackColour);
	glLightfv(GL_LIGHT0, GL_EMISSION, blackColour);
	
	// background is sky-blue 0.5f, 0.8f, 0.92f, 1.0
	glClearColor(0.5f, 0.5f, 0.5f, 1.0);

	// clear the buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// compute the view matrix by combining camera translation, rotation & world2OpenGL

	// Get the camera rotation matrix 
	auto cameraRotationMatrix = Matrix4::Identity();
	auto cameraView = m_camera->GetViewMatrix();
	for(int i = 0; i < 3; ++i)
	{
		for(int j = 0; j < 3; ++j)
		{
			cameraRotationMatrix[j][i] = cameraView[j][i];
		}
	}

	// compute the light position
  	Homogeneous4 lightDirection = world2OpenGLMatrix * cameraRotationMatrix.transpose() * sunDirection;
  	
  	// turn it into Cartesian and normalise
  	Cartesian3 lightVector = lightDirection.Vector().unit();

	// and set the w to zero to force infinite distance
 	lightDirection.w = 0.0;
 	 	
	// pass it to OpenGL
	glLightfv(GL_LIGHT0, GL_POSITION, &(lightVector.x));

	// and set a material colour for the ground
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, groundColour);
	glMaterialfv(GL_FRONT, GL_SPECULAR, blackColour);
	glMaterialfv(GL_FRONT, GL_EMISSION, blackColour);

	// render the terrain
	auto groundMatrix = m_camera->GetViewMatrix() * world2OpenGLMatrix;
	groundModel.Render(groundMatrix);
	
	// now set the colour to draw the bones
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, boneColour);	

	// duration since application started
	auto currentTime = std::chrono::high_resolution_clock::now();
	double nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime - startTime).count();
	double duration = nanoseconds / 1e+9;

	float scale = 0.2f;
	// Player controller
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, playerColour);
	// Set the player matrix for movement in the world. This will allow the player to move and look
	auto playerControllerMatrix = m_camera->GetViewMatrix() * Matrix4::Translate(m_characterPosition) * m_playerLookMatrix * world2OpenGLMatrix * Matrix4::RotateX(-90.0f);
	playerController.Render(playerControllerMatrix, 1.0f, frameNumber, duration, m_characterPosition,m_characterDirection, m_playerLookMatrix);

	// Switch the animation being rendered based on the current state of the player
	// std::cout << "Animation state: " << playerController.m_currentState << std::endl;

	// std::cout << "pos: " << m_characterPosition << std::endl;


	switch (playerController.m_AnimState)
	{
		// If the player is running, render the running animation
		case Running:
			if(playerController.m_currentState != Running)
			{
				playerController.timeStart = std::chrono::high_resolution_clock::now();
				playerController.isTransitioningBack = true;
				playerController.m_currentState = Running;
			}
			break;
		// If the player is turning left, render the turning left animation
		case TurnLeft:
			if(playerController.m_currentState != TurnLeft)
			{
				playerController.timeStart = std::chrono::high_resolution_clock::now();
				playerController.transitionTo.push_back(veerLeftCycle);
				playerController.isTransitioningBack = false;
				playerController.m_currentState = TurnLeft;
				m_characterPosition = playerController.rootPosition;
			}
			break;

		case TurnRight:
			if(playerController.m_currentState != TurnRight)
			{
				playerController.timeStart = std::chrono::high_resolution_clock::now();
				playerController.transitionTo.push_back(veerRightCycle);
				playerController.isTransitioningBack = false;
				playerController.m_currentState = TurnRight;
			}
			break;
			
		case Idle:
			if(playerController.m_currentState != Idle)
			{
				playerController.timeStart = std::chrono::high_resolution_clock::now();
				playerController.transitionTo.push_back(restPose);
				playerController.isTransitioningBack = false;
				playerController.m_currentState = Idle;
			}
			break;

		// // If the player is turning right, render the turning right animation
		// The default state of the player is in rest pose, (Idle)
		default:
			break;
	}


	//std::cout << "Size: " << playerController.transitionTo.size() << std::endl;

	// Debug purposes: Draw view matrix in the scene 
	// auto start = Cartesian3(0.0f, 0.0f, 0.0f);
	// start = m_camera->GetViewMatrix() * start;
	// drawMatrix(m_camera->GetViewMatrix(), m_camera->GetViewMatrix(), start);

	} // Render()	

// camera control events: WASD for motion
void SceneModel::EventCameraForward()
	{ // EventCameraForward()
	// This will move the camera forward in the scene
	m_camera->Forward();
	} // EventCameraForward()

void SceneModel::EventCameraBackward()
	{ // EventCameraBackward()
	// This will allow the user to move the camera back in the scene
	m_camera->Back();
	} // EventCameraBackward()

void SceneModel::EventCameraLeft()
	{ // EventCameraLeft()
	// update the camera matrix
	// This will allow the user to move the camera left in the scene
	m_camera->Left();
	
	} // EventCameraLeft()
	
void SceneModel::EventCameraRight()
	{ // EventCameraRight()
	// update the camera matrix
	// This will allow the user to the camera right in the scene
	m_camera->Right();
	} // EventCameraRight()

// camera control events: RF for vertical motion
void SceneModel::EventCameraUp()
	{ // EventCameraUp()
	// update the camera matrix
	// This will allow the user to move the camera up in the scene
	m_camera->Up();
	} // EventCameraUp()
	
void SceneModel::EventCameraDown()
	{ // EventCameraDown()
	// update the camera matrix
	// This will allow the user to mvoe the camera down in the scene
	m_camera->Down();
	
	} // EventCameraDown()

// camera rotation events: QE for left and right
void SceneModel::EventCameraTurnLeft()
	{ // EventCameraTurnLeft()
	// This will allow the player to look left using camera yaw
	m_camera->YawLeft();
	} // EventCameraTurnLeft()

void SceneModel::EventCameraTurnRight()
	{ // EventCameraTurnRight()
	// This will allow the player to look right using camera yaw
	m_camera->YawRight();
	} // EventCameraTurnRight()

// character motion events: arrow keys for forward, backward, veer left & right
void SceneModel::EventCharacterTurnLeft()
	{ // EventCharacterTurnLeft()
		playerController.m_AnimState = TurnLeft; // set the animation state of the player
		//m_characterDirection.Rotate(-40.0f, Cartesian3(0.0f, 1.0f, 0.0f));
	} // EventCharacterTurnLeft()
	
void SceneModel::EventCharacterTurnRight()
	{ // EventCharacterTurnRight()
		playerController.m_AnimState = TurnRight; // set the new animation state of the player
		// m_characterDirection.Rotate(40.0f, Cartesian3(0.0f, 1.0f, 0.0f));
	} // EventCharacterTurnRight()
	
void SceneModel::EventCharacterForward()
	{ // EventCharacterForward()
		playerController.m_AnimState = Running; // set the animation state of the player
	} // EventCharacterForward()
	
void SceneModel::EventCharacterBackward()
	{ // EventCharacterBackward()
		playerController.m_AnimState = Idle;
	} // EventCharacterBackward()

// reset character to original position: p
void SceneModel::EventCharacterReset()
	{ // EventCharacterReset()
	this->m_characterPosition = Cartesian3(0, 0, 0);
	this->m_playerLookMatrix = Matrix4::Identity(); 
	m_characterDirection = Cartesian3(0.0f, 0.0f, 1.0f); // reset the direction of the character 
	playerController.m_AnimState = Running; // set the animation to be idle
	} // EventCharacterReset()
