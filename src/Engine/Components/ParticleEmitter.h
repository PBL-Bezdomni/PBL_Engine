#pragma once
#include "Component.h"
#include <vector>
#include "glm/glm.hpp"

class ParticleSystem;

class ParticleEmitter : public Component
{
public:
	glm::vec3 MaxVelocity = glm::vec3(2.f, 0.7f, 2.f);
	float VelocityMult = 4;
	glm::vec4 Color = glm::vec4(0.9f);
	glm::vec4 ColorVelocity = glm::vec4(0);
	bool DecreaseAlpha = true;

	float MaxLife = 0.3f;
	float MinLife = 0.1f;
	bool IsRandomLife = false;
	float MaxSize = 0.5f;
	float MinSize = 0.01f;
	bool IsRandomSize = false;

	bool IsRandomPosition = false;
	glm::vec3 RandomPositionOffset;

	void Initialize(const char* vertPath, const char* fragPath, const char* compPath, const char* modelPath, const char* texPath);
	
	void Awake() override;
	void Start() override;
	void Update() override;

	void Play();
	void Stop();
	
	glm::vec3 GetPosition();
	
	glm::vec3 GetPositionOffset();
	void      SetPositionOffset(glm::vec3 offset);

	uint64_t GetID();

	void SetBulk(float bulk);
	void SetSpawnRate(float spawnRate);

private:
	uint64_t m_ID;
	bool m_IsEmitting = false;

	float m_SpawnRate = 30.0f; // particles per second
	float m_Accumulator = 0.0f;
	float m_Bulk = 5;

	ParticleSystem* m_ParticleSystem;

	glm::vec3 m_PositionOffset = glm::vec3(0);

	void FetchParticleSystem();
};
