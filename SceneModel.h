

#ifndef __SCENE_MODEL_H
#define __SCENE_MODEL_H

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include "Terrain.h"
#include "BVHData.h"
#include "Matrix4.h"
#include "Camera.h"
#include <memory.h>
#include <chrono>

// Define enum to set the animation state of the player
// we can then use this to switch to the correct animation depending on the state
enum CharacterState
{
	Running,
	TurnLeft,
	TurnRight,
	Idle
};

class SceneModel										
	{ // class SceneModel
	public:	
	// a terrain model 
	Terrain groundModel;

	// animation cycles (which implicitly have geometric data for a character)
	BVHData restPose;
	BVHData runCycle;
	BVHData veerLeftCycle;
	BVHData veerRightCycle;

	// seperate bvh for the player/character
	BVHData playerController;

	// location & orientation of character
	Cartesian3 characterLocation;
	Matrix4 characterRotation;

	// a matrix that specifies the mapping from world coordinates to those assumed
	// by OpenGL
	Matrix4 world2OpenGLMatrix;

	// matrix for user camera
	Matrix4 viewMatrix;
	Matrix4 CameraTranslateMatrix;
	Matrix4 CameraRotationMatrix;
	
	// the frame number for use in animating
	unsigned long frameNumber;
	
	// constructor
	SceneModel();
	// destructor
	~SceneModel();

	// routine that updates the scene for the next frame
	void Update();

	// routine to tell the scene to render itself
	void Render();

	// camera control events: WASD for motion
	void EventCameraForward();
	void EventCameraLeft();
	void EventCameraRight();
	void EventCameraBackward();
	// camera control events: RF for vertical motion
	void EventCameraUp();
	void EventCameraDown();
	// camera rotation events: QE for left and right
	void EventCameraTurnLeft();
	void EventCameraTurnRight();
	
	// character motion events: arrow keys for forward, backward, veer left & right
	void EventCharacterTurnLeft();
	void EventCharacterTurnRight();
	void EventCharacterForward();
	void EventCharacterBackward();

	// reset character to original position: p
	void EventCharacterReset();

	// needed for now for Xiaoyuan's code
	void EventSwitchMode();

	inline int lerp(float a, float b, float t)
	{
		return a + t  * (b - a);
	}

	// position and direction of the player/character 
	Cartesian3 m_characterPosition;
	Cartesian3 m_characterDirection;
	// look matrix for the player to have him move in the direction it's facing
	Matrix4 m_playerLookMatrix;
	// scene camera
	Camera* m_camera;

	// position of the repeating running cycle 
	Cartesian3 m_controllerLessRunCyclePosition;

	// current animation state of the player/character
	CharacterState m_AnimState;

	}; // class SceneModel

#endif


	
