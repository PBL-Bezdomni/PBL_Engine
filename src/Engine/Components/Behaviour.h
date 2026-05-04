#pragma once
#include "Component.h"

class Behaviour: public Component
{
public:
	Behaviour() = default;
	~Behaviour() override = default;

	void Awake() override;
	void Start() override;
	void Update() override;
};
