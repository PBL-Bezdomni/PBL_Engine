#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glad/glad.h>

enum ECameraMovement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

struct Plane
{
	// unit vector
	glm::vec3 normal = {0.0f, 1.0f, 0.0f };

	// distance from origin to the nearest point in plane
	float distance = 0.f;

	Plane(const glm::vec3& pnt, const glm::vec3& norm)
		: normal(glm::normalize(norm)),
		  distance(glm::dot(normal, pnt))
	{}

	Plane() = default;

	float GetSignedDistanceToPlane(const glm::vec3& point) const
	{
		return glm::dot(normal, point) - distance;
	}
};

struct Frustum
{
	Plane TopFace = Plane();
	Plane BottomFace = Plane();

	Plane RightFace = Plane();
	Plane LeftFace = Plane();

	Plane FarFace = Plane();
	Plane NearFace = Plane();

	Frustum() = default;
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 10.f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;
// Camera restraints
const float MIN_ZOOM = 1.f;
const float MAX_ZOOM = 80.f;
const float MAX_PITCH = 89.f;
// Camera near and far planes
const float NEAR_PLANE = .1f;
const float FAR_PLANE = 200.f;

class Camera
{
public:
	Camera(glm::vec3 position = glm::vec3(0.f, 0.f, 0.f), glm::vec3 up = glm::vec3(0.f, 1.f, 0.f), float yaw = YAW, float pitch = PITCH, float farPlane = FAR_PLANE, float nearPlane = NEAR_PLANE);
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);
	glm::mat4 GetViewMatrix();
	glm::mat4 GetProjectionMatrix();
	
	void UpdateCameraVectors();
	float GetFovY();
	glm::vec3 GetPosition();
	glm::vec3 GetFront();
	glm::vec3 GetUp();
	glm::vec3 GetRight();
	glm::vec3 GetWorldUp();
	float GetYaw();
	float GetPitch();
	float GetZoom();
	void SetZoom(float zoom);
	float GetNearPlane();
	void  SetNearPlane(float nearP);
	float GetFarPlane();
	void  SetFarPlane(float farP);
	float GetAspect();
	void  SetAspect(float aspect);
	Frustum GetFrustum();

	void ProcessKeyboard(ECameraMovement direction, float deltaTime);
	void ProcessMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch = true);
	void ProcessMouseScroll(float yOffset);
private:
	// camera attributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	// euler angles
	float Yaw;
	float Pitch;
	// camera options
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;

	// far and near plane;
	float FarPlane;
	float NearPlane;
	float Aspect;

	Frustum CameraFrustum;

	void UpdateFrustum();
};