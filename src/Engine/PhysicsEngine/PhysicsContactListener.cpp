#include "PhysicsContactListener.h"
#include "GameObject.h"
#include <spdlog/spdlog.h>
#include "../Components/Behaviour.h"
#include "Game/Scripts/MassageTable.h"
#include "Engine/Engine.h"

PhysicsContactListener::Pair PhysicsContactListener::MakePair(JPH::BodyID a, JPH::BodyID b)
{
    if (a < b)
    {
        return {a, b};
    }
    return {b, a};
}

void PhysicsContactListener::OnContactAdded(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings)
{
    Pair pair = MakePair(inBody1.GetID(), inBody2.GetID());
    if (++m_ContactCounts[pair] == 1)
    {

        GameObject* obj1 = reinterpret_cast<GameObject*>(inBody1.GetUserData());
        GameObject* obj2 = reinterpret_cast<GameObject*>(inBody2.GetUserData());

        if (!obj1 || !obj2) return;

        bool isSensor1 = inBody1.IsSensor();
        bool isSensor2 = inBody2.IsSensor();

        if (isSensor1)
        {
            vector<Behaviour*> scripts = obj1->GetVectorOfComponents<Behaviour>();
            for (Behaviour* script : scripts)
            {
                if (script != nullptr)
                {
                    script->OnTriggerEnter(obj2);
                }
            }
        }

        if (isSensor2)
        {
            vector<Behaviour*> scripts = obj2->GetVectorOfComponents<Behaviour>();
            for (Behaviour* script : scripts)
            {
                if (script != nullptr)
                {
                    script->OnTriggerEnter(obj1);
                }
            }
        }
        
        ContactInfo         info = ContactInfo(obj1, obj2);
        m_ActiveContacts.emplace(pair, info);
    }
}

void PhysicsContactListener::OnContactPersisted(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings)
{
}

void PhysicsContactListener::OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair)
{
    Pair pair = MakePair(inSubShapePair.GetBody1ID(), inSubShapePair.GetBody2ID());

    auto countIt = m_ContactCounts.find(pair);

    if (countIt == m_ContactCounts.end()) return;

    if (--countIt->second == 0)
    {
        auto it = m_ActiveContacts.find(pair);

        if (it != m_ActiveContacts.end())
        {
            GameObject* obj1 = it->second.gameObject1;
            GameObject* obj2 = it->second.gameObject2;

            if (!obj1 || !obj2) return;

            vector<Behaviour*> scripts = obj1->GetVectorOfComponents<Behaviour>();
            for (Behaviour* script : scripts)
            {
                if (script != nullptr)
                {
                    script->OnTriggerExit(obj2);
                }
            }

            scripts = obj2->GetVectorOfComponents<Behaviour>();
            for (Behaviour* script : scripts)
            {
                if (script != nullptr)
                {
                    script->OnTriggerExit(obj1);
                }
            }

            m_ActiveContacts.erase(it);
        }
        m_ContactCounts.erase(countIt);
    }
}
