#include "ParticleEmitter.h"

void ParticleEmitter::Awake()
{
	Component::Awake();
	m_TotalParticles = m_NumParticlesX * m_NumParticlesY * m_NumParticlesZ;
}

void ParticleEmitter::InitBuffers()
{
	std::vector<glm::vec4> Positions(m_TotalParticles);
}
