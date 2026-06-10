#pragma once
#include "Engine/Components/Behaviour.h"
#include "Engine/AssetManager.h"
#include "Game/SceneManager.h"

class GameObject;

class TutorialArrow : public Behaviour
{
private:
    AssetManager* m_AssetMgr;
    SceneManager* m_SceneMgr;
    Camera* m_MainCamera;
    bool m_AnimateArrow;

    float m_PositionOffset = 0.2f;
    float m_AnimationTimer = 0;
    int m_AnimationSign = 1;
    glm::vec3 m_StartPosition = glm::vec3(0.f, 2.0f, 0.f);
    const float ANIMATION_TIME = 1.5f;

    // GameObject* m_Target = nullptr;

    std::shared_ptr<Shader> m_ArrowShader;
    std::shared_ptr<GameObject> m_ArrowObject;

public:
    void Awake() override;
    void DrawUpdate() override;
    void Update() override;

    void UpdateArrowShader();

    void SetStartPosition(glm::vec3 pos);

    void SetActive(bool active);
};