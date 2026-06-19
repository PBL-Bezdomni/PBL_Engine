#pragma once
#include <memory>
#include <vector>
#include <map>
#include "Camera.h"
#include "Component.h"
#include "ParticleEmitter.h"
#include "ParticleRenderer.h"
#include "Shader.h"

class AssetManager;

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

class Model;

class ParticleSystem : public Component
{
public:
	void Awake() override;
	void Update() override;
	void DrawUpdate() override;

	uint64_t CreateEmitter(const char* vertPath, const char* fragPath, const char* compPath, const char* modelPath, const char* texPath);
	void Emit(ParticleEmitter& emitter, uint32_t count);
private:
	const uint32_t MAX_PARTICLES = 100;
	uint32_t m_NextEmitterID = 0;

	AssetManager* m_AssetMgr;
	std::vector<Particle> m_Particles;
	// std::shared_ptr<Shader> m_ParticleGraphicShader;
	// std::shared_ptr<Shader> m_ParticleComputeShader;
	std::map<uint64_t, GLuint> m_BuffersMap;
	std::map<uint64_t, std::shared_ptr<Shader>> m_GraphicalShaderMap;
	std::map<uint64_t, std::shared_ptr<Shader>> m_ComputeGraphicalShaderMap;
	std::vector<std::shared_ptr<GameObject>> m_Renderers;
	// GLuint m_SSBO;
	uint32_t m_NextParticle = 0;

	Camera* m_MainCamera;

	GLuint InitialBuffers();
	void Dispatch();
};
