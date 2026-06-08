#include "TutorialArrow.h"
#include "GameObject.h"
#include "Engine/Time.h"
#include "Game/SceneManager.h"
#include "Engine/AssetManager.h"
#include <glm/glm.hpp>
#include "Engine/Components/Component.h"
#include "Engine/Engine.h"

void TutorialArrow::Awake()
{
    Behaviour::Awake();

    Engine& engine = Engine::GetInstance();
    m_AssetMgr = &engine.GetAssetManager();
    m_SceneMgr = &engine.GetGameManager().GetSceneManager();
    m_MainCamera = m_SceneMgr->GetMainCamera().get();

    m_ArrowShader = m_AssetMgr->TutorialArrowShader;
    m_ArrowObject = m_SceneMgr->Instantiate(m_Owner, "res/models/ProgressBarPlane.obj", m_ArrowShader);
    m_ArrowObject->Name = "TestArrow";

    Model* arrowModel = m_ArrowObject->GetComponent<Model>();
    if (arrowModel != nullptr)
    {
        arrowModel->ReassignShader(*m_ArrowShader);
        arrowModel->AssignTexture(*m_AssetMgr->GetTexture("res/textures/UI/arrowIcon.png"));
    }

    m_ArrowObject->transform->Scale = glm::vec3(1.0f);
    m_ArrowObject->transform->Position = glm::vec3(0.0f, m_StartPosition, 0.0f);
    SetActive(false);
}


void TutorialArrow::DrawUpdate()
{
    Behaviour::DrawUpdate();

    if (m_ArrowObject == nullptr || m_Owner == nullptr) return;

    m_ArrowShader->Use();
    m_ArrowShader->SetFloat("u_width", 1.0);
    m_ArrowShader->SetFloat("u_height", 1.0);
    m_ArrowShader->SetVec3("cameraRight", m_MainCamera->GetRight());
    m_ArrowShader->SetVec3("cameraUp", m_MainCamera->GetUp());
    m_ArrowShader->SetFloat("rotationAngle", glm::pi<float>() / 2);
}

void TutorialArrow::Update()
{
    Behaviour::Update();
    if (m_AnimateArrow)
    {
        float y = lerp(-m_AnimationSign * m_PositionOffset, m_AnimationSign * m_PositionOffset, m_AnimationTimer / ANIMATION_TIME);
        m_AnimationTimer += Time::GetDeltaTime();
        if (m_AnimationTimer >= ANIMATION_TIME)
        {
            m_AnimationTimer = 0;
            m_AnimationSign *= -1;
        }
        m_ArrowObject->transform->Position.y = m_StartPosition + y;
    }
}

void TutorialArrow::UpdateArrowShader()
{
    if (m_ArrowObject != nullptr && m_MainCamera != nullptr)
    {
        m_AssetMgr->TutorialArrowShader->Use();
    }
}

void TutorialArrow::SetActive(bool active)
{
    m_ArrowObject->SetActive(active);
    m_AnimateArrow = active;
}
