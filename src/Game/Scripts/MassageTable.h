#pragma once
#include "AOnsenObject.h"

class MassageTable : public AOnsenObject
{
public:
    void Awake() override;

    const char* GetScriptName() const override { return "MassageTable"; }
};