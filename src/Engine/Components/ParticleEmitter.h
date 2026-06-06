#pragma once
#include "Component.h"
#include <vector>
#include "glm/glm.hpp"

class ParticleSystem;

class ParticleEmitter : public Component
{
public:
	glm::vec3 MaxVelocity = glm::vec3(2.f, 0.2f, 2.f);
	float VelocityMult = 1;
	glm::vec4 Color = glm::vec4(0.9f);
	glm::vec4 ColorVelocity = glm::vec4(0);
	bool DecreaseAlpha = true;

	float MaxLife = 0.3f;
	float MinLife = 0.1f;
	bool IsRandomLife = false;
	float MaxSize = 0.5f;
	float MinSize = 0.01f;
	bool IsRandomSize = false;
	
	void Awake() override;
	void Start() override;
	void Update() override;

	void Play();
	void Stop();
	
	glm::vec3 GetPosition();
	
	glm::vec3 GetPositionOffset();
	void      SetPositionOffset(glm::vec3 offset);

private:
	bool m_IsEmitting = false;

	float spawnRate = 20.0f; // particles per second
	float accumulator = 0.0f;

	ParticleSystem* m_ParticleSystem;

	glm::vec3 m_PositionOffset = glm::vec3(0);
};
