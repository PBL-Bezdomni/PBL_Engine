#include "CameraManager.h"

#include "GameObject.h"
// #include "Player.h"

void CameraManager::AssignCameraAndPlayer(Camera* cam, GameObject* player)
{
	m_Camera = cam;
	m_Player = player;
}

void CameraManager::Update()
{
	Behaviour::Update();
	if (m_Camera != nullptr && m_Player != nullptr)
	{
		glm::vec3 camPos = m_Camera->GetPosition();
		m_Camera->SetPosition(glm::vec3(m_Player->transform->Position.x, camPos.y, camPos.z));
	}
}
