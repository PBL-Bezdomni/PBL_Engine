#include "PhysicsContactListener.h"
#include "GameObject.h"
#include <spdlog/spdlog.h>
#include "../Components/Behaviour.h"
#include "Game/Scripts/MassageTable.h"

void PhysicsContactListener::OnContactAdded(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings)
{
    GameObject* obj1 = reinterpret_cast<GameObject*>(inBody1.GetUserData());
    GameObject* obj2 = reinterpret_cast<GameObject*>(inBody2.GetUserData());

    if (!obj1 || !obj2) return;

    bool isSensor1 = inBody1.IsSensor();
    bool isSensor2 = inBody2.IsSensor();

    if (isSensor1)
    {
        Behaviour* script = obj1->GetDerivedComponent<Behaviour>();
        if (script != nullptr)
        {
            script->OnTriggerEnter(obj2);
        }
    }

    if (isSensor2)
    {
        Behaviour* script = obj2->GetDerivedComponent<Behaviour>();
        if (script != nullptr)
        {
            script->OnTriggerEnter(obj1);
        }
    }
}

void PhysicsContactListener::OnContactPersisted(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings)
{

}