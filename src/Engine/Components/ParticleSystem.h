#pragma once
#include <memory>

#include "Camera.h"
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
	bool m_IsCPU = false;
	const uint32_t MAX_PARTICLES = 50000;

	std::vector<Particle> m_Particles;
	std::shared_ptr<Shader> m_ParticleGraphicShader;
	std::shared_ptr<Shader> m_ParticleComputeShader;
	GLuint m_SSBO;
	uint32_t m_NextParticle = 0;

	Camera* m_MainCamera;

	void InitialBuffers();
	void Dispatch();
	void DispatchCPU();
};
