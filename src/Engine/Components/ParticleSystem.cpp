#include "ParticleSystem.h"

#include "ParticleEmitter.h"
#include "Random.h"
#include "Engine/AssetManager.h"
#include "Engine/Engine.h"
#include "Engine/Time.h"

void ParticleSystem::Awake()
{
	Component::Awake();
	Engine* engine = &Engine::GetInstance();
	m_AssetMgr = &engine->GetAssetManager();
	SceneManager* sm = &engine->GetGameManager().GetSceneManager();

	m_Owner->Name = "ParticleSystem";

	m_Particles.resize(MAX_PARTICLES);

	for (auto& particle : m_Particles)
	{
		particle.position = glm::vec4(0.0f);
		particle.velocity = glm::vec4(0.0f);

		particle.color = glm::vec4(0.0f);
		particle.life = 0.0f;
		particle.maxLife = 0.0f;
		particle.size = 0.0f;
		particle.alive = 0;
	}

	m_MainCamera = sm->GetMainCamera().get();
}

void ParticleSystem::Update()
{
	Component::Update();
	Dispatch();
}

uint64_t ParticleSystem::CreateEmitter(const char* vertPath, const char* fragPath, const char* compPath, const char* modelPath, const char* texPath)
{
	m_NextEmitterID++;
	
	m_GraphicalShaderMap[m_NextEmitterID] = m_AssetMgr->GetShader(vertPath, fragPath);
	m_ComputeGraphicalShaderMap[m_NextEmitterID] = m_AssetMgr->GetComputeShader(compPath);
	m_BuffersMap[m_NextEmitterID] = InitialBuffers();

	vector<glm::mat4>      instanceMatrix(MAX_PARTICLES);
	shared_ptr<Shader>     shader = m_GraphicalShaderMap.at(m_NextEmitterID);
	std::shared_ptr<GameObject> emitter = make_shared<GameObject>();
	emitter->Name = compPath;
	m_Owner->AddChild(emitter.get());
	ParticleRenderer* pr = emitter->AddComponent<ParticleRenderer>();
	m_Renderers.push_back(emitter);
	pr->Initialize(shader, modelPath, texPath, MAX_PARTICLES, instanceMatrix, m_BuffersMap[m_NextEmitterID]);

	return m_NextEmitterID;
}

void ParticleSystem::Emit(ParticleEmitter& emitter, uint32_t count)
{
	uint64_t id = emitter.GetID();
	GLuint ssbo = m_BuffersMap.at(id);
	
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);

	glm::vec3 position = emitter.GetPosition();
	if (emitter.IsRandomPosition)
	{
		float offsetX = Random::GetRandomFloat(-1, 1);
		float offsetY = Random::GetRandomFloat(-1, 1);
		float offsetZ = Random::GetRandomFloat(-1, 1);
		position += glm::vec3(emitter.RandomPositionOffset.x * offsetX, emitter.RandomPositionOffset.y * offsetY, emitter.RandomPositionOffset.z * offsetZ);
	}

	Particle* particles = (Particle*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(Particle) * MAX_PARTICLES, GL_MAP_WRITE_BIT);

	if (!particles) return;
	
	for (uint32_t i = 0; i < count; i++)
	{
		Particle& p = particles[m_NextParticle];
		p.position = glm::vec4(position, 1.0f);

		glm::vec3 vel = emitter.MaxVelocity;
		vel *= emitter.VelocityMult;
		p.velocity = glm::vec4(
			Random::GetRandomFloat(-vel.x, vel.x),
			Random::GetRandomFloat(-vel.y, vel.y),
			Random::GetRandomFloat(-vel.z, vel.z),
			0.0f
		);

		p.color = emitter.Color;
		p.life = emitter.MaxLife;
		p.maxLife = emitter.MaxLife;
		p.size = emitter.MaxSize;
		p.alive = 1;

		m_NextParticle = (m_NextParticle + 1) % MAX_PARTICLES;
	}

	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
}

GLuint ParticleSystem::InitialBuffers()
{
	GLuint ssbo;
	glGenBuffers(1, &ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Particle) * MAX_PARTICLES, m_Particles.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
	return ssbo;
}

void ParticleSystem::Dispatch()
{
	for (auto [id, shader] : m_ComputeGraphicalShaderMap)
	{
		GLuint ssbo = m_BuffersMap.at(id);
		shader->Use();
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0 , ssbo);
		shader->SetFloat("deltaTime", Time::GetDeltaTime());
		glDispatchCompute((MAX_PARTICLES + 255) / 256, 1, 1);
		// glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT |	GL_BUFFER_UPDATE_BARRIER_BIT);
	}
}
