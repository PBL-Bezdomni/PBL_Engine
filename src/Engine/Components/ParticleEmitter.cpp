#include "ParticleEmitter.h"
#include "ParticleSystem.h"
#include "Random.h"
#include "Engine/Engine.h"
#include "Engine/Time.h"

void ParticleEmitter::Initialize(const char* vertPath, const char* fragPath, const char* compPath, const char* modelPath, const char* texPath)
{
	FetchParticleSystem();
	if (m_ParticleSystem != nullptr)
	{
		m_ID = m_ParticleSystem->CreateEmitter(vertPath, fragPath, compPath, modelPath, texPath);
	}
}

void ParticleEmitter::Awake()
{
	Component::Awake();
	FetchParticleSystem();
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

		m_Accumulator += m_SpawnRate * deltaTime;
		uint32_t count = static_cast<uint32_t>(m_Accumulator);

		if (count >= m_Bulk)
		{
			m_Accumulator -= count;

			if (m_ParticleSystem != nullptr)
			{
				m_ParticleSystem->Emit(*this, count);
			}
			// If emitter is not set to loop, stop it after first bulk is emitted.
			if (!m_Loop)
			{
				Stop();
			}
		}
	}
}

void ParticleEmitter::Play()
{
	if (!m_IsEmitting)
	{
		m_IsEmitting = true;
		if (m_ParticleSystem == nullptr)
		{
			FetchParticleSystem();
		}
	}
}

void ParticleEmitter::Stop()
{
	m_IsEmitting = false;
}

glm::vec3 ParticleEmitter::GetPosition()
{
	return m_Owner->transform->GetGlobalPosition() + m_PositionOffset;
}

glm::vec3 ParticleEmitter::GetPositionOffset()
{
	return m_PositionOffset;
}

void ParticleEmitter::SetPositionOffset(glm::vec3 offset)
{
	m_PositionOffset = offset;
}

uint64_t ParticleEmitter::GetID()
{
	return m_ID;
}

void ParticleEmitter::SetBulk(float bulk)
{
	m_Bulk = bulk;
}

void ParticleEmitter::SetSpawnRate(float spawnRate)
{
	m_SpawnRate = spawnRate;
}

void ParticleEmitter::SetLoop(bool loop)
{
	m_Loop = loop;
}

void ParticleEmitter::FetchParticleSystem()
{
	if (m_ParticleSystem == nullptr)
	{
		m_ParticleSystem = Engine::GetInstance().GetGameManager().GetSceneManager().GetParticleSystem();
	}
}
