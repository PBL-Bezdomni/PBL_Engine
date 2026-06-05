#pragma once
#include <memory>

#include "Component.h"
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
private:
	constexpr uint32_t MAX_PARTICLES = 10000;

	std::vector<Particle> m_Particles;
	std::shared_ptr<Shader> m_ParticlesShader;
	GLuint m_SSBO;

	void InitialBuffers();
	void Dispatch();
	void DrawUpdate();
};
