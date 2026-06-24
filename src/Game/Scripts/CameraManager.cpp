#include "CameraManager.h"
#include "GameObject.h"
#include <algorithm>
#include "Engine/Time.h"

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
        float dt = Time::GetDeltaTime();

        glm::vec3 pos1 = m_Player1->transform->Position;
        glm::vec3 pos2 = m_Player2->transform->Position;
        float targetCenterX = (pos1.x + pos2.x) / 2.0f;

        glm::vec3 currentCamPos = m_Camera->GetPosition();

        float smoothedX = glm::mix(currentCamPos.x, targetCenterX, m_SmoothSpeed * dt);

        m_Camera->SetPosition(glm::vec3(smoothedX, currentCamPos.y, currentCamPos.z));

        float distance = glm::distance(pos1, pos2);

        float minZoom = 30.0f;
        float maxZoom = 80.0f;

        float zoomMultiplier = 0.4f;

        float marginBuffer = 30.0f;

        float targetZoom = minZoom + marginBuffer + (distance * zoomMultiplier);
        targetZoom = std::clamp(targetZoom, minZoom, maxZoom);

        float currentZoom = m_Camera->GetZoom();
        float smoothedZoom = glm::mix(currentZoom, targetZoom, m_ZoomSmoothSpeed * dt);

        m_Camera->SetZoom(smoothedZoom);
	}
}
