#pragma once
#include "Component.h"
#include <memory>
#include <vector>
#include "Shader.h"

class Camera;

class ParticleRenderer : public Component
{
public:
	void Initialize(std::shared_ptr<Shader> shader, const char* modelPath, const char* texPath, uint32_t maxParticles, std::vector<glm::mat4> instanceMatrix, GLuint ssbo);

	void DrawUpdate() override;
	void AfterDrawUpdate() override;
private:
	GLuint m_SSBO;
	std::shared_ptr<Shader> m_GraphicShader;
	Camera* m_MainCamera;
};
