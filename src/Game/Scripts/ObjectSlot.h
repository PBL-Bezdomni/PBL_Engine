#pragma once
#include "Animal.h"

class ObjectSlot
{
public:
	bool IsOccupied = false;
	Animal* OccupyingAnimal = nullptr;
	glm::vec3 LocalOffset;
};
