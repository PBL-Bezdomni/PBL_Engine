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

	virtual void OnTriggerEnter(GameObject* other) {}
	virtual void OnTriggerStay(GameObject* other) {}
	virtual void OnTriggerExit(GameObject* other) {}

	virtual void OnCollisionEnter(GameObject* other) {}
	virtual void OnCollisionStay(GameObject* other) {}
	virtual void OnCollisionExit(GameObject* other) {}
};
