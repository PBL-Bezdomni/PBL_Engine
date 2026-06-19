#pragma once
#include "AOnsenObject.h"

class Bath : public AOnsenObject
{
public:
    void Awake() override;

    // void OnTriggerEnter(GameObject* other) override;
    // void OnTriggerExit(GameObject* other) override;

    const char* GetScriptName() const override { return "Bath"; }

protected:
    void AssignParticles() override;
};