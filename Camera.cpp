#include "Camera.h"

Camera::Camera(const Cartesian3& position, const Cartesian3& direction, const Cartesian3& up)
{
    m_cameraPositon = position;
    m_cameraDirection = direction;
    m_yaw = 0.0f;
    
    m_cameraSpeed = 2.0f;
    auto forward = Cartesian3(0, 0, -1);

    forward.Rotate(m_yaw, up);
    forward = forward.unit();

    Cartesian3 x = up;
    x = x.cross(forward);
    x = x.unit();

    Cartesian3 u = forward.cross(x);
    u = u.unit();

    m_cameraDirection = forward;

    m_viewMatrix = Matrix4::ViewMatrix(m_cameraPositon, m_cameraPositon + m_cameraDirection, up);
}

// Update the camera each frame as the user is using it 
void Camera::Update()
{   
    auto up = Cartesian3(0, 1.0f, 0.0f);
    auto forward = Cartesian3(0, 0, -1);

    forward.Rotate(m_yaw, up);
    forward = forward.unit();

    Cartesian3 x = up;
    x = x.cross(forward);
    x = x.unit();

    Cartesian3 u = forward.cross(x);
    u = u.unit();

    m_cameraDirection = forward;

    m_viewMatrix = Matrix4::ViewMatrix(m_cameraPositon, m_cameraPositon + m_cameraDirection, up);
}

// Move the camera forward
void Camera::Forward()
{
    m_cameraPositon = m_cameraPositon + m_cameraSpeed * m_cameraDirection;
}

// Move the camera back
void Camera::Back()
{
    m_cameraPositon = m_cameraPositon - m_cameraSpeed * m_cameraDirection;
}

// Move the camera up 
void Camera::Up()
{
    m_cameraPositon.y += m_cameraSpeed;
}

// Move the camera down
void Camera::Down()
{
    m_cameraPositon.y -= m_cameraSpeed; 
}

// Move the camera to the right
void Camera::Right()
{
    // Use direction and up to get right
    auto right = m_cameraDirection.cross(Cartesian3(0, 1.0f, 0.0f));
    right = right.unit();

    m_cameraPositon = m_cameraPositon + right * m_cameraSpeed;
}

// Move the camera to the left
void Camera::Left()
{
    // Use direction and up to get right
    auto right = m_cameraDirection.cross(Cartesian3(0, 1.0f, 0.0f));
    right = right.unit();
    m_cameraPositon = m_cameraPositon - right * m_cameraSpeed;
}

// QE for yaw right and left 
// Allow camera to look to the right
void Camera::YawRight()
{
    m_yaw -= m_cameraSpeed;
}

// Allow the camera to look to the left
void Camera::YawLeft()
{
    m_yaw += m_cameraSpeed;
}
