#include "Matrix4.h"

// Camera class to have a moveable camera in the scene
class Camera
{
public:
    Camera();
    // constructor takes an initial position, direction and up 
    Camera(const Cartesian3& position, const Cartesian3& direction, const Cartesian3& up);

    // return the view matrix we construct using camera properties
    Matrix4 GetViewMatrix() const { return m_viewMatrix; } 
    // return the camera's current position
    Cartesian3 GetPosition() const { return m_cameraPositon; }
    // return the camera's current direction
    Cartesian3 GetDirection() const { return m_cameraDirection; }

    // Update the camera each frame
    void Update();
    // Camera controls
    void Forward();
    void Back();
    void Up();
    void Down();
    void Right();
    void Left();

    // QE for yaw right and left 
    void YawRight();
    void YawLeft();

private:
    float m_cameraSpeed; // movement speed of the camera
    float m_yaw = 0.0f;
    Cartesian3 m_cameraPositon; // position of the camera
    Cartesian3 m_cameraDirection; // direction of the camera
    Matrix4 m_viewMatrix; // view matrix from camera properties
};