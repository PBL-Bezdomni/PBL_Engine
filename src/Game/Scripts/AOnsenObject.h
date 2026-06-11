#pragma once
#include <vector>
// #include "ObjectSlot.h"
#include "ObjectSlot.h"
#include "Engine/Components/Behaviour.h"
#include "glm/vec3.hpp"

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
public:
	void Awake() override;

	void OnTriggerEnter(GameObject* other) override;
	void OnTriggerExit(GameObject* other) override;

	void RemoveAnimal(Animal* animal);
	Animal* GetAnimalForPlayer(glm::vec3 rayStart, glm::vec3 rayDir);

	AnimalNeeds GetNeed();

	TutorialArrow* GetTutorialArrow();
};
