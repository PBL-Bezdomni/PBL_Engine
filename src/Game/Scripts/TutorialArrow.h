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
    float m_StartPosition = 2.0f;
    const float ANIMATION_TIME = 1.5f;

    // GameObject* m_Target = nullptr;

    std::shared_ptr<Shader> m_ArrowShader;
    std::shared_ptr<GameObject> m_ArrowObject;

public:
    void Awake() override;
    void DrawUpdate() override;
    void Update() override;

    void UpdateArrowShader();

    void SetActive(bool active);
};