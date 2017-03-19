#include "Common.h"
#include "PhysicsListener.h"
#include "SceneManager.h"

namespace BONE_GRAPHICS
{
    void PhysicsEventListener::beginContact(const ContactPointInfo& contact)
    {
        auto Object1 = SceneMgr->CurrentScene()->FindObjectByName(contact.collisionShape1->name);
        auto Object2 = SceneMgr->CurrentScene()->FindObjectByName(contact.collisionShape2->name);

        if (Object1 != nullptr && Object2 != nullptr)
        {
            Object1->CollisionEvent(Object2);
            Object2->CollisionEvent(Object1);
        }
    }
}