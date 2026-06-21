#include "ParticleRenderer.h"
#include "Model.h"
#include "Engine/Loader.h"
#include "GameObject.h"
#include "Engine/AssetManager.h"
#include "Engine/Engine.h"
#include "Camera.h"

void ParticleRenderer::Initialize(std::shared_ptr<Shader> shader, const char* modelPath, const char* texPath, uint32_t maxParticles, std::vector<glm::mat4> instanceMatrix, GLuint ssbo)
{
	Engine* engine = &Engine::GetInstance();
	AssetManager* am = &engine->GetAssetManager();
	m_MainCamera = engine->GetGameManager().GetSceneManager().GetMainCamera().get();
	
	m_GraphicShader = shader;
	Model         billboard = Model(*m_GraphicShader, (Loader::RelativePath() + modelPath).c_str(), maxParticles, instanceMatrix);
	m_Owner->AddComponent<Model>(billboard);
	// Just to be sure
	m_Owner->GetComponent<Model>()->ReassignShader(*m_GraphicShader);
	Texture       tex = *am->GetTexture(texPath);
	m_Owner->GetComponent<Model>()->AssignTexture(tex);

	m_SSBO = ssbo;
}

void ParticleRenderer::DrawUpdate()
{
	Component::DrawUpdate();
	m_GraphicShader->Use();
	glEnable(GL_BLEND);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO);
	m_GraphicShader->SetVec3("cameraRight", m_MainCamera->GetRight());
	m_GraphicShader->SetVec3("cameraUp", m_MainCamera->GetUp());
}

void ParticleRenderer::AfterDrawUpdate()
{
	Component::AfterDrawUpdate();
	glDisable(GL_BLEND);
}
