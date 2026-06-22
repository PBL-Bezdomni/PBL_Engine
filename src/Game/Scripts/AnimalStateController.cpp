#include "AnimalStateController.h"
#include "Animal.h"
#include "Engine/Time.h"
#include <Engine/Animation/Animator.h>

AnimalStateController::AnimalStateController(Animal* animal) : m_Owner(animal)
{
	m_EventBinder.Bind(RequestStateChange, [this](AnimalState newState) { this->ChangeStateInternal(newState); });
}

void AnimalStateController::ChangeStateInternal(AnimalState newState)
{
	if (m_CurrentState == newState) return;

	AnimalState oldState = m_CurrentState;
	m_CurrentState = newState;
	m_StateTimer = 0.0f;

	if (m_Owner && m_Owner->GetGameObject())
	{
		Animator* animator = m_Owner->GetGameObject()->GetComponent<Animator>();
		if (animator != nullptr)
		{
			switch (m_CurrentState)	
			{
			case AnimalState::None:
			case AnimalState::Idle:
			case AnimalState::PickedUp:
			case AnimalState::Throw:
			case AnimalState::Rest:
			case AnimalState::CheckIn:
				animator->PlayAnimation("idle");
				break;
			case AnimalState::Walking:
				animator->PlayAnimation("walk");
				break;
			case AnimalState::Chasing:
				animator->PlayAnimation("run");
				break;
			case AnimalState::Eating:
				animator->PlayAnimation("eat");
				break;
			default:
				break;
			}
		}
	}

	OnStateChanged.Invoke(oldState, newState);
}

void AnimalStateController::Update()
{
	if (!m_Owner) return;

	m_StateTimer += Time::GetDeltaTime();

	switch (m_CurrentState)
	{
	case AnimalState::Idle:
		m_Owner->UpdateIdle();
		break;
	case AnimalState::Walking:
		m_Owner->UpdateWalking();
		break;
	case AnimalState::Chasing:
		m_Owner->UpdateChasing();
		break;
	case AnimalState::Eating:
		m_Owner->UpdateEating();
		if (m_StateTimer >= 1.5f)
		{
			RequestStateChange.Invoke(AnimalState::Idle);
		}
		break;
	case AnimalState::PickedUp:
		m_Owner->UpdatePickedUp();
		break;
	case AnimalState::Throw:
		m_Owner->UpdateThrow();

		if (m_CurrentState != AnimalState::None) return;

		if (m_StateTimer >= 3.0f)
		{
			RequestStateChange.Invoke(AnimalState::Idle);
		}
		break;
	case AnimalState::Rest:
		m_Owner->UpdateFulfillingNeed();
		break;
	case AnimalState::CheckIn:
		m_Owner->UpdateCheckIn();

		if (m_StateTimer >= 1.5f)
		{
			RequestStateChange.Invoke(AnimalState::Idle);
		}
		break;
	case AnimalState::None:
	default:
		break;
	}
}