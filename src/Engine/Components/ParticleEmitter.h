#pragma once
#include "Component.h"
#include <vector>
#include "ParticleSystem.h"
#include "glm/glm.hpp"


class ParticleEmitter : public Component
{
public:
	void Awake() override;
	void Start() override;
	void Update() override;

	void Play();
	void Stop();
private:
	bool m_IsEmitting = false;

	float spawnRate = 20.0f; // particles per second
	float accumulator = 0.0f;

	ParticleSystem* m_ParticleSystem;
};
