#include "Bath.h"
#include "Engine/Components/ParticleEmitter.h"

void Bath::Awake()
{
    m_ObjectNeed = AnimalNeeds::Bath;
    AOnsenObject::Awake();
}

void Bath::AssignParticles()
{
    ParticleEmitter* emitter = m_Owner->AddComponent<ParticleEmitter>();
    emitter->Initialize("res/shaders/basicParticles.vert", "res/shaders/basicParticles.frag", "res/shaders/basicParticles.comp", "res/models/PieChartPlane.obj", "res/textures/UI/bubble.png");
    emitter->SetSpawnRate(30);
    emitter->SetBulk(1);
    emitter->MaxVelocity = glm::vec3(0.f, 0.3f, 0.f);
    emitter->MaxLife = 0.5f;
    emitter->Color = glm::vec4(0.258f, 0.59f, 0.711f, .2f);
    emitter->SetPositionOffset(glm::vec3(0, -6.2, 0));
    emitter->IsRandomPosition = true;
    emitter->RandomPositionOffset = glm::vec3(5, 0, 5);
    emitter->Play();
}

