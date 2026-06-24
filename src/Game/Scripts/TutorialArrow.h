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

    float m_PositionOffset = 0.9f;
    float m_AnimationTimer = 0;
    int m_AnimationSign = 1;
    glm::vec3 m_StartPosition = glm::vec3(0.f, 4.0f, 0.f);
    glm::vec3 m_ArrowColor = glm::vec3(1.0f, 1.0f, 1.0f);
    const float ANIMATION_TIME = 0.6f;
    glm::vec3 m_Player1Color = glm::vec3(0.18f, 0.36f, 0.54f);
    glm::vec3 m_Player2Color = glm::vec3(0.25f, 0.42f, 0.31f);
    bool m_Is1Active = false;
    bool m_Is2Active = false;

    // GameObject* m_Target = nullptr;

    std::shared_ptr<Shader> m_ArrowShader;
    std::shared_ptr<GameObject> m_ArrowObject;

    bool IsGivenPlayerActive(int id);
    bool IsOtherPlayerActive(int id);
    void SetGivenPlayerActive(bool active, int id);
    void SetGivenPlayerColor(int id);
    void SetOtherPlayerColor(int id);

public:
    void Awake() override;
    void DrawUpdate() override;
    void Update() override;

    void UpdateArrowShader();

    void SetStartPosition(glm::vec3 pos);

    void SetActive(bool active, int id);

    
};