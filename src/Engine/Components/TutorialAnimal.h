#pragma once
#include "Engine/Components/Behaviour.h"
#include "Engine/AssetManager.h"
#include "Game/SceneManager.h"

class GameObject;

class TutorialAnimal : public Behaviour
{
private:
    AssetManager* m_AssetMgr;
    SceneManager* m_SceneMgr;
    Camera* m_MainCamera;

    GameObject* m_Target = nullptr;

    std::shared_ptr<Shader> m_ArrowShader;
    std::shared_ptr<GameObject> m_ArrowObject;

public:
    void Awake() override;
    void DrawUpdate() override;

    void UpdateArrowShader();

    void SetTarget(GameObject* target);
    void ClearTarget();
};