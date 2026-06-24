#pragma once
#include <vector>
// #include "ObjectSlot.h"
#include "ObjectSlot.h"
#include "Engine/Components/Behaviour.h"
#include "glm/vec3.hpp"
#include "Engine/AssetManager.h"
#include "Game/SceneManager.h"

class TutorialArrow;
class Animal;

class AOnsenObject : public Behaviour
{
protected:
	int m_MaxSlots = 4;
	std::vector<ObjectSlot> m_Slots;

	bool m_IsOccupied = false;
	GameObject* m_OccupyingAnimal = nullptr;
	TutorialArrow* m_TutorialArrow;

	std::vector<glm::vec3> m_SlotsPos = {
		glm::vec3(-3.0f, -4.f, -3.0f),
		glm::vec3(3.0f, -4.f, -3.0f),
		glm::vec3(-3.0f, -4.f, 3.0f),
		glm::vec3(3.0f, -4.f, 3.0f)
	};

	AnimalNeeds m_ObjectNeed = AnimalNeeds::Bath;

	virtual void AssignParticles();

	bool m_IsSquareIndicator = false;
	float m_IndicatorScale = 20.0f;
	glm::vec3 m_IndicatorOffset = glm::vec3(0.0f, 0.0f, 0.0f);
	std::shared_ptr<GameObject> m_Indicator;

	std::string m_IconTexturePath = "";
	glm::vec3 m_IconScale = glm::vec3(2.0f);
	glm::vec3 m_IconOffset = glm::vec3(0.0f, 4.0f, 0.0f);
	std::shared_ptr<GameObject> m_IconObject = nullptr;
	std::shared_ptr<Shader> m_IconShader = nullptr;

public:
	void Awake() override;

	void OnTriggerEnter(GameObject* other) override;
	void OnTriggerExit(GameObject* other) override;

	void RemoveAnimal(Animal* animal);
	Animal* GetAnimalForPlayer(glm::vec3 rayStart, glm::vec3 rayDir);

	AnimalNeeds GetNeed();

	TutorialArrow* GetTutorialArrow();
	void DrawUpdate() override;
	std::vector<Animal*> GetAnimalsInObject();
private:
	AssetManager* m_AssetMgr;
	SceneManager* m_SceneMgr;
	std::shared_ptr<Shader> m_PieShader;
};
