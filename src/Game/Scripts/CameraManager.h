#pragma once
#include "Camera.h"
#include "Engine/Components/Behaviour.h"
// #include "Game/Scripts/Player.h"
// class Player;

class CameraManager : public Behaviour
{
private:
	Camera* m_Camera;
	GameObject* m_Player1;
	GameObject* m_Player2;

	float m_SmoothSpeed = 0.8f;
	float m_ZoomSmoothSpeed = 0.8f;
public:
	void AssignCameraAndPlayer(Camera* cam, GameObject* player1, GameObject* player2);

	void Update() override;	
};
