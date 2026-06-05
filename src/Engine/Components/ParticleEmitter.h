#pragma once
#include "Component.h"
#include <vector>
#include "glm/glm.hpp"

class ParticleEmitter : public Component
{
public:
	void Awake() override;
	void Start() override;
	void Update() override;

	void StartParticles();
private:
	const int MAX_PARTICLES = 10000;

	int m_NumParticlesX = 100;
	int m_NumParticlesY = 1;
	int m_NumParticlesZ = 100;
	int m_TotalParticles;

	void InitBuffers();
};
