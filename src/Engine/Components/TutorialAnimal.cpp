#include "TutorialAnimal.h"
#include "GameObject.h"
#include "Engine/Time.h"
#include "Game/SceneManager.h"
#include "Engine/AssetManager.h"
#include <glm/glm.hpp>
#include "../Components/Component.h"
#include "../Engine.h"

void TutorialAnimal::Awake()
{
    Behaviour::Awake();

    Engine& engine = Engine::GetInstance();
    m_AssetMgr = &engine.GetAssetManager();
    m_SceneMgr = &engine.GetGameManager().GetSceneManager();
    m_MainCamera = m_SceneMgr->GetMainCamera().get();

    m_ArrowObject = m_SceneMgr->Instantiate(m_Owner, "res/models/TutorialArrowPlane.obj", m_AssetMgr->TutorialArrowShader);
    m_ArrowObject->Name = "TestArrow";

    Model* arrowModel = m_ArrowObject->GetComponent<Model>();
    if (arrowModel != nullptr)
    {
        arrowModel->AssignTexture(*m_AssetMgr->GetTexture("res/textures/UI/arrowIcon.png"));
    }

    m_ArrowObject->transform->Scale = glm::vec3(3.0f);
    m_ArrowObject->transform->Position = glm::vec3(0.0f, 5.0f, 0.0f);
    m_ArrowObject->transform->EulerAngles = glm::vec3(90.0f, 0.0f, 0.0f);
    m_ArrowObject->SetActive(true);
}

void TutorialAnimal::SetTarget(GameObject* target)
{
    m_Target = target;
    if (m_ArrowObject != nullptr && m_Target != nullptr)
    {
        m_ArrowObject->SetActive(true);
    }
}

void TutorialAnimal::ClearTarget()
{
    m_Target = nullptr;
    if (m_ArrowObject != nullptr)
    {
        m_ArrowObject->SetActive(false);
    }
}


void TutorialAnimal::DrawUpdate()
{
    Behaviour::DrawUpdate();

    if (m_ArrowObject == nullptr || m_Owner == nullptr) return;

    glm::vec3 targetPos = (m_Target != nullptr) ? m_Target->GetWorldPosition() : glm::vec3(-20.0, 0.0f, 25.0f);
    glm::vec3 arrowWorldPos = m_ArrowObject->GetWorldPosition();

    glm::vec3 diff = targetPos - arrowWorldPos;
    diff.y = 0.0f;

    if (glm::length(diff) > 0.1f)
    {
        glm::vec3 dir = glm::normalize(diff);
        float targetYaw = glm::degrees(atan2(dir.x, dir.z));

        float parentYaw = m_Owner->transform->EulerAngles.y;
        float localYaw = targetYaw - parentYaw;

        m_ArrowObject->transform->EulerAngles = glm::vec3(0.0f, localYaw + 180.0f, 0.0f);
    }

    m_ArrowObject->UpdateSelfAndChild();
}

void TutorialAnimal::UpdateArrowShader()
{
    if (m_ArrowObject != nullptr && m_MainCamera != nullptr)
    {
        m_AssetMgr->TutorialArrowShader->Use();
    }
}