#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch, float farPlane, float nearPlane)
{
	Position = position;
	WorldUp = up;
	Yaw = yaw;
	Pitch = pitch;

	Front = glm::vec3(0.f, 0.f, -1.f);
	MovementSpeed = SPEED;
	MouseSensitivity = SENSITIVITY;
	Zoom = ZOOM;

	FarPlane = farPlane;
	NearPlane = nearPlane;

	UpdateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
{
	Position = glm::vec3(posX, posY, posZ);
	WorldUp = glm::vec3(upX, upY, upZ);
	Yaw = yaw;
	Pitch = pitch;

	Front = glm::vec3(0.f, 0.f, -1.f);
	MovementSpeed = SPEED;
	MouseSensitivity = SENSITIVITY;
	Zoom = ZOOM;

	FarPlane = FAR_PLANE;
	NearPlane = NEAR_PLANE;

	UpdateCameraVectors();
}

void Camera::UpdateFrustum()
{
	Frustum f;
	float fovY = GetFovY();
	const float halfVSide = FarPlane * tanf(fovY * 0.5f);
	const float halfHSide = halfVSide * Aspect;
	const glm::vec3 frontMultFar = FarPlane * Front;
	
	f.NearFace = Plane(Position + NearPlane * Front, Front);
	f.FarFace = Plane(Position + frontMultFar, -Front);
	f.RightFace = Plane(Position, glm::cross(frontMultFar - Right * halfHSide, Up));
	f.LeftFace = Plane(Position, glm::cross(Up, frontMultFar + Right * halfHSide));
	f.TopFace = Plane(Position, glm::cross(Right, frontMultFar - Up * halfVSide));
	f.BottomFace = Plane(Position, glm::cross(frontMultFar + Up * halfVSide, Right));

	CameraFrustum = f;
}

glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(Position, Position + Front, Up);
}

glm::mat4 Camera::GetProjectionMatrix()
{
	return glm::perspective(glm::radians(Zoom), Aspect, NearPlane, FarPlane);
}

void Camera::ProcessKeyboard(ECameraMovement direction, float deltaTime)
{
	float velocity = MovementSpeed * deltaTime;
	if (direction == FORWARD)
	{
		Position += Front * velocity;
	}
	if (direction == BACKWARD)
	{
		Position -= Front * velocity;
	}
	if (direction == LEFT)
	{
		Position -= Right * velocity;
	}
	if (direction == RIGHT)
	{
		Position += Right * velocity;
	}
}

void Camera::ProcessMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch)
{
	xOffset *= MouseSensitivity;
	yOffset *= MouseSensitivity;

	Yaw += xOffset;
	Pitch += yOffset;

	if (constrainPitch)
	{
		if (Pitch > MAX_PITCH)
		{
			Pitch = MAX_PITCH;
		}
		if (Pitch < -MAX_PITCH)
		{
			Pitch = -MAX_PITCH;
		}
	}

	UpdateCameraVectors();
}

void Camera::ProcessMouseScroll(float yOffset)
{
	Zoom -= (float)yOffset;
	if (Zoom < MIN_ZOOM)
	{
		Zoom = MIN_ZOOM;
	}
	if (Zoom > MAX_ZOOM)
	{
		Zoom = MAX_ZOOM;
	}
}

void Camera::UpdateCameraVectors()
{
	// calculate the new Front vector
	glm::vec3 front;
	front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	front.y = sin(glm::radians(Pitch));
	front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	Front = glm::normalize(front);
	// also re-calculate the Right and Up vector
	Right = glm::normalize(glm::cross(Front, WorldUp));
	Up = glm::normalize(glm::cross(Right, Front));

	UpdateFrustum();
}

float Camera::GetFovY()
{
	return glm::radians(Zoom);
}

glm::vec3 Camera::GetPosition()
{
	return Position;
}

glm::vec3 Camera::GetFront()
{
	return Front;	
}

glm::vec3 Camera::GetUp()
{
	return Up;	
}

glm::vec3 Camera::GetRight()
{
	return Right;
}

glm::vec3 Camera::GetWorldUp()
{
	return WorldUp;
}

float Camera::GetYaw()
{
	return Yaw;
}

float Camera::GetPitch()
{
	return Pitch;
}

float Camera::GetZoom()
{
	return Zoom;
}

void Camera::SetZoom(float zoom)
{
	Zoom = zoom;
}

void Camera::SetNearPlane(float nearP)
{
	NearPlane = nearP;
}

float Camera::GetNearPlane()
{
	return NearPlane;
}

void Camera::SetFarPlane(float farP)
{
	FarPlane = farP;
}

float Camera::GetFarPlane()
{
	return FarPlane;
}

void Camera::SetAspect(float aspect)
{
	Aspect = aspect;
}

float Camera::GetAspect()
{
	return Aspect;
}

Frustum Camera::GetFrustum()
{
	return CameraFrustum;
}
