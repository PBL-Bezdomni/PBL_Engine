#include "ParticleEmitter.h"
#include "ParticleSystem.h"
#include "Engine/Engine.h"
#include "Engine/Time.h"

void ParticleEmitter::Awake()
{
	Component::Awake();
	m_ParticleSystem = Engine::GetInstance().GetGameManager().GetSceneManager().GetParticleSystem();
}

void ParticleEmitter::Start()
{
	Component::Start();
}

void ParticleEmitter::Update()
{
	Component::Update();
	if (m_IsEmitting)
	{
		float deltaTime = Time::GetDeltaTime();

		accumulator += spawnRate * deltaTime;
		uint32_t count = static_cast<uint32_t>(accumulator);

		accumulator -= count;

		if (m_ParticleSystem != nullptr)
		{
			m_ParticleSystem->Emit(*this, count);
		}
	}
}

void ParticleEmitter::Play()
{
	m_IsEmitting = true;
	if (m_ParticleSystem == nullptr)
	{
		m_ParticleSystem = Engine::GetInstance().GetGameManager().GetSceneManager().GetParticleSystem();
	}
}

void ParticleEmitter::Stop()
{
	m_IsEmitting = false;
}

glm::vec3 ParticleEmitter::GetPosition()
{
	return m_Owner->transform->GetGlobalPosition();
}
