#include "Sauna.h"

#include "Engine/Components/ParticleEmitter.h"

void Sauna::Awake()
{
	m_ObjectNeed = AnimalNeeds::Sauna;
	AOnsenObject::Awake();

	m_IsSquareIndicator = false;
	m_IndicatorScale = 50.0f;
	m_IndicatorOffset = glm::vec3(0.0f, -20.0f, 0.0f);
	m_Indicator->transform->Scale = glm::vec3(m_IndicatorScale);
	m_Indicator->transform->Position = m_IndicatorOffset;
}

void Sauna::AssignParticles()
{
	ParticleEmitter* emitter = m_Owner->AddComponent<ParticleEmitter>();
	emitter->Initialize("res/shaders/basicParticles.vert", "res/shaders/basicParticles.frag", "res/shaders/basicParticles.comp", "res/models/PieChartPlane.obj", "res/textures/UI/smoke.png");
	emitter->SetSpawnRate(20);
	emitter->SetBulk(3);
	emitter->MaxVelocity = glm::vec3(0.f, 5.f, 0.f);
	emitter->MaxLife = 1.2f;
	emitter->MaxSize = 1.f;
	emitter->Color = glm::vec4(0.804f, 0.804f, 0.804f, .5f);
	emitter->SetPositionOffset(glm::vec3(0, -4, 0));
	emitter->IsRandomPosition = true;
	emitter->RandomPositionOffset = glm::vec3(3, 0, 3);
	emitter->Play();
}
