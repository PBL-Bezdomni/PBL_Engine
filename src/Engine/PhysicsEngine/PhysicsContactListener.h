#pragma once
#include <mutex>
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/ContactListener.h>
#include <Jolt/Physics/Body/Body.h>

#include "GameObject.h"
#include "Jolt/Physics/Body/BodyPair.h"

class GameObject;

class PhysicsContactListener : public JPH::ContactListener
{
private:
    // Structs for some bullshit that didn't work at the end
    struct ContactInfo
    {
        GameObject* gameObject1;
        GameObject* gameObject2;

        ContactInfo(GameObject* g1, GameObject* g2)
        {
            gameObject1 = g1;
            gameObject2 = g2;
        }
    };
    
    struct Pair
    {
        JPH::BodyID body1;
        JPH::BodyID body2;

        bool operator==(const Pair&) const = default;
    };

    // For hashing pair, otherwise maps didn't compile.
    struct PairHash
    {
        size_t operator()(const Pair& p) const
        {
            size_t h1 = std::hash<uint32_t>{}(p.body1.GetIndexAndSequenceNumber());
            size_t h2 = std::hash<uint32_t>{}(p.body2.GetIndexAndSequenceNumber());

            return h1 ^ (h2 << 1);
        }
    };

    // Map I used to keep info about gameObjects. So I didn't need to get locked Jolt bodys, I could directly just update gameObjects
    std::unordered_map<Pair, ContactInfo, PairHash> m_ActiveContacts;
    // Map I used to track if contacts where added more times, but It unsurprisingly didn't work, since OnTriggerEnter isn't called more than once.
    std::unordered_map<Pair, int, PairHash> m_ContactCounts;

    // Makes pair, making sure that lower id is always first
    Pair MakePair(JPH::BodyID a, JPH::BodyID b);
    std::mutex m_Mutex;
public:
    virtual void OnContactAdded(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings) override;

    virtual void OnContactPersisted(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings) override;

    virtual void OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair) override;
};