#pragma once
#include <memory>
#include "Component.h"
#include "ParticleEmitter.h"
#include "Shader.h"

struct Particle
{
	glm::vec4 position;
	glm::vec4 velocity;

	glm::vec4 color;
	float life;
	float maxLife;
	float size;
	uint32_t alive;
};

class ParticleSystem : public Component
{
public:
	void Awake() override;
	void Update() override;
	void DrawUpdate() override;
	
	void Emit(ParticleEmitter& emitter, uint32_t count);
private:
	const uint32_t MAX_PARTICLES = 100;

	std::vector<Particle> m_Particles;
	std::shared_ptr<Shader> m_ParticleGraphicShader;
	std::shared_ptr<Shader> m_ParticleComputeShader;
	GLuint m_SSBO;
	uint32_t m_NextParticle = 0;

	float XVelocity = 0.5f;
	float YVelocity = 0.2f;

	void InitialBuffers();
	void Dispatch();
};
