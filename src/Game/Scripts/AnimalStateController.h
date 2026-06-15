#pragma once

#include "Engine/Events/AEvent.h"
#include "Engine/Events/EventBinder.h"

enum class AnimalState
{
	None,
	Idle,
	PickedUp,
	Throw,
	Rest,
	CheckIn
};

class Animal;

class AnimalStateController
{
private:
	AnimalState m_CurrentState = AnimalState::None;
	Animal* m_Owner;
	
	float m_StateTimer = 0.0f;

	void ChangeStateInternal(AnimalState newState);

	EventBinder m_EventBinder;
public:
	AnimalStateController(Animal* animal);
	~AnimalStateController() = default;

	AEvent<AnimalState> RequestStateChange;
	AEvent<AnimalState, AnimalState> OnStateChanged;

	void Update();
	
	AnimalState GetCurrentState() const { return m_CurrentState; }
	float GetStateTimer() const { return m_StateTimer; }
};