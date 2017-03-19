#pragma once
#include "Common.h"

namespace BONE_GRAPHICS
{
    class PhysicsEventListener : public reactphysics3d::EventListener {
    public:
        virtual void beginContact(const ContactPointInfo& contact);

        /// Called when a new contact point is found between two bodies
        /**
        * @param contact Information about the contact
        */
        virtual void newContact(const ContactPointInfo& contact) {}

        /// Called at the beginning of an internal tick of the simulation step.
        /// Each time the DynamicsWorld::update() method is called, the physics
        /// engine will do several internal simulation steps. This method is
        /// called at the beginning of each internal simulation step.
        virtual void beginInternalTick() {}

        /// Called at the end of an internal tick of the simulation step.
        /// Each time the DynamicsWorld::update() metho is called, the physics
        /// engine will do several internal simulation steps. This method is
        /// called at the end of each internal simulation step.
        virtual void endInternalTick() {}
    };
}