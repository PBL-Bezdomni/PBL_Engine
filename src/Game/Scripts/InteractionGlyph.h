#pragma once
#include <memory>

#include "Camera.h"
#include "Shader.h"
#include "Texture.h"
#include "Engine/Components/Component.h"

class InteractionGlyph : public Component
{
private:
	const float Y_POS = 17.f;
	const float OFFSET = 3;
	const float DURATION = 0.6f;
	
	Camera* m_MainCamera;
	std::shared_ptr<Shader> m_InteractionMarkShader;
	
	int m_Sign = 1;
	float m_Min = Y_POS - OFFSET;
	float m_Max = Y_POS + OFFSET;

	float m_Timer = 0;

public:
	void Update() override;
	void DrawUpdate() override;
	void Awake() override;
	void Initialize(std::shared_ptr<Shader> shader, Texture tex);
};
