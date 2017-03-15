#pragma once
#include "ISingleton.h"
#include "Common.h"
#include "Collision.h"

namespace BONE_GRAPHICS
{
    class PhysicsManager : public ISingleton<PhysicsManager>, public BONE_SYSTEM::MultiThreadSync<PhysicsManager>
    {
    private:
        std::thread* physics_thread;

        void UpdatePhysics();
        bool isInit;

        std::list<Collision*> collisionList;

        bool PointToRect(const Vector2& pos, const RECT& rect);
        bool VectorToAABB(const Vector3& pos, const TAG_BOX& box);

        bool AABBToAABB(const TAG_BOX& box1, const TAG_BOX& box2);
        bool VectorToSphere(const Vector3& pos, const TAG_SPHERE& sphere);
        bool SphereToSphere(const TAG_SPHERE& sphere1, const TAG_SPHERE& sphere2);
        bool AABBToOBB(const TAG_BOX& box, const TAG_OBB& obb);
        bool OBBToOBB(const TAG_OBB& obb, const TAG_OBB& obb2);

    public:
        PhysicsManager() {}
        virtual ~PhysicsManager() {}

        void InitializeMembers();
        virtual void ReleaseMembers();

        bool AddCollision(Collision* collision);
        bool RemoveCollision(Collision* collision);
    };
}