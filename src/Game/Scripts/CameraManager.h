#pragma once
#include "Camera.h"
#include "Engine/Components/Behaviour.h"
// #include "Game/Scripts/Player.h"
// class Player;

class CameraManager : public Behaviour
{
private:
	Camera* m_Camera;
	GameObject* m_Player;
public:
	void AssignCameraAndPlayer(Camera* cam, GameObject* player);

	void Update() override;	
};
