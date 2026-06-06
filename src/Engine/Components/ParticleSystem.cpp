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
	AssetManager* am = &engine->GetAssetManager();
	SceneManager* sm = &engine->GetGameManager().GetSceneManager();

	m_Owner->Name = "ParticleSystem";

	m_ParticleGraphicShader = am->GetShader("res/shaders/smokeParticles.vert", "res/shaders/smokeParticles.frag");
	m_ParticleComputeShader = am->GetComputeShader("res/shaders/smokeParticles.comp");

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

	InitialBuffers();

	// Create Model for shader
	// not used, but I do it, because I don't know what will happen without it and don't want to test it now
	vector<glm::mat4> instanceMatrix(MAX_PARTICLES);
	Model billboard = Model(*m_ParticleGraphicShader, (Loader::RelativePath() + "res/models/PieChartPlane.obj").c_str(), MAX_PARTICLES, instanceMatrix);
	m_Owner->AddComponent<Model>(billboard);
	// Just to be sure
	m_Owner->GetComponent<Model>()->ReassignShader(*m_ParticleGraphicShader);
	Texture tex = *am->GetTexture("res/textures/UI/smoke.png");
	m_Owner->GetComponent<Model>()->AssignTexture(tex);

	m_MainCamera = sm->GetMainCamera().get();
}

void ParticleSystem::Update()
{
	Component::Update();
	Dispatch();
}

void ParticleSystem::DrawUpdate()
{
	Component::DrawUpdate();
	m_ParticleGraphicShader->Use();
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO);
	m_ParticleGraphicShader->SetVec3("cameraRight", m_MainCamera->GetRight());
	m_ParticleGraphicShader->SetVec3("cameraUp", m_MainCamera->GetUp());

	// Particle* data = (Particle*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
	//
	// data[0].alive = 0;
	// data[1].alive = 0;
	//
	// glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
}

void ParticleSystem::Emit(ParticleEmitter& emitter, uint32_t count)
{
	// m_ParticlesShader->Use();
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO);

	glm::vec3 position = emitter.GetPosition();
	
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

void ParticleSystem::InitialBuffers()
{
	glGenBuffers(1, &m_SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Particle) * MAX_PARTICLES, m_Particles.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_SSBO);
}

void ParticleSystem::Dispatch()
{
	m_ParticleComputeShader->Use();
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0 , m_SSBO);
	m_ParticleComputeShader->SetFloat("deltaTime", Time::GetDeltaTime());
	glDispatchCompute((MAX_PARTICLES + 255) / 256, 1, 1);
	// glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT |
				GL_BUFFER_UPDATE_BARRIER_BIT);
}