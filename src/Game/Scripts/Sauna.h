#pragma once
#include "AOnsenObject.h"

class Sauna : public AOnsenObject
{
public:
	void Awake() override;

	// void OnTriggerEnter(GameObject* other) override;
	// void OnTriggerExit(GameObject* other) override;
	
	const char* GetScriptName() const override { return "Sauna"; }

protected:
	void AssignParticles() override;
};
