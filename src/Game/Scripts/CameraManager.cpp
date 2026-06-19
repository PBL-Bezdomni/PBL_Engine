#include "CameraManager.h"
#include "GameObject.h"
#include <algorithm>

void CameraManager::AssignCameraAndPlayer(Camera* cam, GameObject* player1, GameObject* player2)
{
	m_Camera = cam;
	m_Player1 = player1;
	m_Player2 = player2;
}

void CameraManager::Update()
{
	Behaviour::Update();

	if (m_Camera != nullptr && m_Player1 != nullptr && m_Player2 != nullptr)
	{
		glm::vec3 pos1 = m_Player1->transform->Position;
		glm::vec3 pos2 = m_Player2->transform->Position;

		float centerX = (pos1.x + pos2.x) / 2.0f;

		float distance = glm::distance(pos1, pos2);

		float minZoom = 50.0f;
		float maxZoom = 80.0f;
		float zoomMultiplier = 0.1f;

		float targetZoom = minZoom + (distance * zoomMultiplier);

		targetZoom = std::clamp(targetZoom, minZoom, maxZoom);

		glm::vec3 camPos = m_Camera->GetPosition();

		m_Camera->SetPosition(glm::vec3(centerX, camPos.y, camPos.z));

		m_Camera->SetZoom(targetZoom);
	}
}
