#include "Bath.h"
#include "Engine/Components/ParticleEmitter.h"

void Bath::Awake()
{
    m_ObjectNeed = AnimalNeeds::Bath;

    m_IsSquareIndicator = false;
    m_IndicatorScale = 50.0f;
    m_IndicatorOffset = glm::vec3(0.0f, -20.0f, 0.0f);

    m_IconTexturePath = "res/textures/UI/objects/bath.png";
    m_IconScale = glm::vec3(3.0f);
    m_IconOffset = glm::vec3(0.0f, 50.0f, 10.0f);

    AOnsenObject::Awake();
}

void Bath::AssignParticles()
{
    ParticleEmitter* emitter = m_Owner->AddComponent<ParticleEmitter>();
    emitter->Initialize("res/shaders/basicParticles.vert", "res/shaders/basicParticles.frag", "res/shaders/basicParticles.comp", "res/models/primitives/plane.obj", "res/textures/UI/bubble.png");
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

