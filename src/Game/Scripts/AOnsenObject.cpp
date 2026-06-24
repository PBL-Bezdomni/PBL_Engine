#include "AOnsenObject.h"
#include "Animal.h"
#include "TutorialArrow.h"
#include "Engine/Components/ParticleEmitter.h"
#include "glm/vec3.hpp"
#include <Engine/Engine.h>
#include "GameObject.h"
#include "Engine/AssetManager.h"

void AOnsenObject::Awake()
{
	Behaviour::Awake();

	for (int i = 0; i < m_MaxSlots; i++)
	{
		m_Slots.push_back(ObjectSlot());
		m_Slots[i].LocalOffset = m_SlotsPos[i];
	}

	m_TutorialArrow = m_Owner->AddComponent<TutorialArrow>();
	AssignParticles();

	m_AssetMgr = &Engine::GetInstance().GetAssetManager();
	m_SceneMgr = &Engine::GetInstance().GetGameManager().GetSceneManager();

	m_PieShader = m_AssetMgr->PieChartShader;

	m_Indicator = m_SceneMgr->Instantiate(m_Owner, "res/models/primitives/plane.obj", m_AssetMgr->PieChartShader);
	m_Indicator->Name = "ObjectIndicator";
	m_Indicator->transform->Position = m_IndicatorOffset;

	m_Indicator->transform->Scale = glm::vec3(m_IndicatorScale);

	m_PieShader->Use();
	m_PieShader->SetInt("u_IsHollow", 1);
	m_PieShader->SetInt("u_IsSquare", m_IsSquareIndicator);
	m_PieShader->SetInt("u_NumNeeds", 1);

	m_PieShader->SetIVec4("u_Needs", static_cast<int>(m_ObjectNeed), -1, -1, -1);
}

void AOnsenObject::OnTriggerEnter(GameObject* other)
{
	Animal* animal = other->GetComponent<Animal>();
	if (animal == nullptr) return;
	if (!animal->m_WasDroppedByPlayer) return;
	animal->m_WasDroppedByPlayer = false;
	for (int i = 0; i < m_MaxSlots; i++)
	{
		if (m_Slots[i].OccupyingAnimal == animal)
		{
			return;
		}
	}

	vector<AnimalNeeds> needs = animal->GetRequiredServices();
	bool objectInNeeds = false;
	for (AnimalNeeds need : needs)
	{
		if (need == m_ObjectNeed)
		{
			objectInNeeds = true;
			break;
		}
	}

	if (!objectInNeeds)
	{
		return;
	}
	
	for (int i = 0; i < m_MaxSlots; i++)
	{
		if (!m_Slots[i].IsOccupied)
		{
			m_Slots[i].IsOccupied = true;
			m_Slots[i].OccupyingAnimal = animal;

			glm::vec3 tubWorldPos = m_Owner->GetWorldPosition();
			glm::vec3 targetPos = tubWorldPos + m_Slots[i].LocalOffset;

			animal->EnterPosition(targetPos);
			animal->StartFulfillingNeed(m_ObjectNeed);
			animal->m_CurrentOnsen = this;
			break;
		}
	}
}

void AOnsenObject::OnTriggerExit(GameObject* other)
{
	 Animal* animal = other->GetComponent<Animal>();
	 if (animal == nullptr) return;
	
	 for (int i = 0; i < m_MaxSlots; i++)
	 {
	 	if (m_Slots[i].OccupyingAnimal == animal)
	 	{
	 		m_Slots[i].IsOccupied = false;
	 		m_Slots[i].OccupyingAnimal = nullptr;
	
	 		animal->StopFulfillingNeed();
	 		animal->m_CurrentOnsen = nullptr;
	 		return;
	 	}
	 }
}

void AOnsenObject::RemoveAnimal(Animal* animal)
{
	if (animal == nullptr) return;
	for (int i = 0; i < m_MaxSlots; i++)
	{
		if (m_Slots[i].OccupyingAnimal == animal)
		{
			m_Slots[i].IsOccupied = false;
			m_Slots[i].OccupyingAnimal = nullptr;
			animal->m_CurrentOnsen = nullptr;
			animal->StopFulfillingNeed();
			return;
		}
	}
}

Animal* AOnsenObject::GetAnimalForPlayer(glm::vec3 rayStart, glm::vec3 rayDir)
{
	Animal* closestAnimal = nullptr;
	float minDistance = 999.0f;

	glm::vec3 worldPos = m_Owner->GetWorldPosition();

	for (int i = 0; i < m_MaxSlots; i++)
	{
		if (m_Slots[i].IsOccupied && m_Slots[i].OccupyingAnimal != nullptr)
		{
			glm::vec3 animalPos = worldPos + m_Slots[i].LocalOffset;
			glm::vec3 wek = animalPos - rayStart;
			
			float dot = glm::dot(wek, rayDir);
			if (dot < 0.0f) continue;

			glm::vec3 projection = rayStart + rayDir * dot;

			float distToRay = glm::length(animalPos - projection);

			if (distToRay < minDistance)
			{
				minDistance = distToRay;
				closestAnimal = m_Slots[i].OccupyingAnimal;
			}
		}
	}
	return closestAnimal;
}

AnimalNeeds AOnsenObject::GetNeed()
{
	return m_ObjectNeed;
}

TutorialArrow* AOnsenObject::GetTutorialArrow()
{
	return m_TutorialArrow;
}

void AOnsenObject::AssignParticles()
{
}

void AOnsenObject::DrawUpdate()
{
	Behaviour::DrawUpdate();

	if (m_PieShader != nullptr && m_Indicator != nullptr && m_Indicator->IsActive())
	{
		m_PieShader->Use();

		m_PieShader->SetInt("u_IsHollow", 1);

		m_PieShader->SetInt("u_IsSquare", m_IsSquareIndicator);

		m_PieShader->SetInt("u_NumNeeds", 1);
		m_PieShader->SetIVec4("u_Needs", static_cast<int>(m_ObjectNeed), -1, -1, -1);
	}
}
