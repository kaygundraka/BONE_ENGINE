#include "Common.h"
#include "PhyscisManager.h"
#include "LogManager.h"

namespace BONE_GRAPHICS
{
    void PhysicsManager::InitializeMembers()
    {
        isInit = true;

        physics_thread = new thread(&PhysicsManager::UpdatePhysics, this);

        LogMgr->ShowMessage(LOG_MESSAGE, "PhysicsManager is initialized");
    }
    
    void PhysicsManager::ReleaseMembers()
    {
        isInit = false;

        physics_thread->join();

        LogMgr->ShowMessage(LOG_MESSAGE, "PhysicsManager is clear.");
    }

    bool PhysicsManager::AddCollision(Collision* collision)
    {
        ThreadSync sync;

        collisionList.push_back(collision);

        return true;
    }

    bool PhysicsManager::RemoveCollision(Collision* collision)
    {
        ThreadSync sync;

        for (auto iter = collisionList.begin(); iter != collisionList.end(); iter++)
        {
            if (*iter == collision)
            {
                collisionList.erase(iter);
                return true;
            }
        }

        return false;
    }

    void PhysicsManager::UpdatePhysics()
    {
        while (Is_init)
        {
            ThreadSync sync;

            for (auto iter = collisionList.begin(); iter != collisionList.end(); iter++)
                (*iter)->Update();

            auto iter1 = collisionList.begin();

            while(iter1 != collisionList.end())
            {
                auto iter2 = iter1;
                iter2++;

                auto iter1_type = (*iter1)->GetCollisionType();
                auto iter1_collision = (*iter1)->GetCollision();

                while (iter2 != collisionList.end())
                {
                    bool crush = false;
                    auto iter2_type = (*iter2)->GetCollisionType();
                    auto iter2_collision = (*iter2)->GetCollision();

                    switch (iter1_type)
                    {
                    case Collision::COLLISION_TYPE::AABB:
                        if (iter2_type == Collision::COLLISION_TYPE::AABB)
                            if (this->AABBToAABB(*(TAG_BOX*)iter1_collision, *(TAG_BOX*)iter2_collision)) crush = true;
                        else if (iter2_type == Collision::COLLISION_TYPE::OBB)
                            if (this->AABBToOBB(*(TAG_BOX*)iter1_collision, *(TAG_OBB*)iter2_collision)) crush = true;
                        break;

                    case Collision::COLLISION_TYPE::SPHERE:
                        if (iter2_type == Collision::COLLISION_TYPE::SPHERE)
                            if (this->SphereToSphere(*(TAG_SPHERE*)iter1_collision, *(TAG_SPHERE*)iter2_collision)) crush = true;
                        break;

                    case Collision::COLLISION_TYPE::OBB:
                        if (iter2_type == Collision::COLLISION_TYPE::AABB)
                            if (this->AABBToAABB(*(TAG_BOX*)iter2_collision, *(TAG_BOX*)iter1_collision)) crush = true;
                        else if (iter2_type == Collision::COLLISION_TYPE::OBB)
                            if (this->AABBToOBB(*(TAG_BOX*)iter2_collision, *(TAG_OBB*)iter1_collision)) crush = true;
                        break;

                    default:
                        break;
                    }

                    if (crush)
                    {
                        (*iter1)->GetGameObject()->OnCollisionStay((*iter2)->GetGameObject());
                        (*iter2)->GetGameObject()->OnCollisionStay((*iter1)->GetGameObject());
                    }
                }
            }
        }
    }

    bool PhysicsManager::PointToRect(const Vector2& pos, const RECT& rect)
    {
        ThreadSync sync;

        if (pos.x > rect.right)	    return false;
        if (pos.x < rect.left)	    return false;
        if (pos.y < rect.top)	    return false;
        if (pos.y > rect.bottom)	return false;

        return true;
    }

    bool PhysicsManager::VectorToAABB(const Vector3& pos, const TAG_BOX& box)
    {
        ThreadSync sync;

        if (pos.x > box.rightTop.x)	    return false;
        if (pos.x < box.leftBottom.x)	return false;
        if (pos.y > box.rightTop.y)	    return false;
        if (pos.y < box.leftBottom.y)	return false;
        if (pos.z > box.rightTop.z)	    return false;
        if (pos.z < box.leftBottom.z)	return false;

        return true;
    }

    bool PhysicsManager::AABBToAABB(const TAG_BOX& box1, const TAG_BOX& box2)
    {
        ThreadSync sync;
        
        if (box1.leftBottom.x + box1.pivot.x  > box2.rightTop.x + box2.pivot.x
            || box1.rightTop.x + box1.pivot.x < box2.leftBottom.x + box2.pivot.x)
            return false;

        if (box1.leftBottom.y + box1.pivot.y > box2.rightTop.y + box2.pivot.y
            || box1.rightTop.y + box1.pivot.y < box2.leftBottom.y + box2.pivot.y)
            return false;

        if (box1.leftBottom.z + box1.pivot.z > box2.rightTop.z + box2.pivot.z
            || box1.rightTop.z + box1.pivot.z < box2.leftBottom.z + box2.pivot.z)
            return false;

        return true;
    }

    bool PhysicsManager::VectorToSphere(const Vector3& pos, const TAG_SPHERE& sphere)
    {
        ThreadSync sync;

        Vector3 RVec(Vector3(0, 0, 0));

        D3DXVec3Subtract(&RVec, &(pos), &(sphere.pivot));

        float C = D3DXVec3Dot(&RVec, &RVec);
        float distans = sphere.radius * sphere.radius;

        if (distans > C)
            return true;
        else
            return false;
    }

    bool PhysicsManager::SphereToSphere(const TAG_SPHERE& sphere1, const TAG_SPHERE& sphere2)
    {
        ThreadSync sync;

        Vector3 RVec(Vector3(0, 0, 0));

        D3DXVec3Subtract(&RVec, &(sphere1.pivot), &(sphere2.pivot));

        float C = D3DXVec3Length(&RVec);
        float distans = sphere1.radius + sphere2.radius;

        if (distans >= C)
            return true;
        else
            return false;
    }

    bool PhysicsManager::AABBToOBB(const TAG_BOX& box, const TAG_OBB& obb)
    {
        ThreadSync sync;

        TAG_OBB OBB;
        OBB.pivot = (box.leftBottom + box.rightTop) / 2.0f;

        Vector3 vX(box.rightTop.x, box.leftBottom.y, box.leftBottom.z);
        Vector3 vY(box.leftBottom.x, box.rightTop.y, box.leftBottom.z);
        Vector3 vZ(box.leftBottom.x, box.leftBottom.y, box.rightTop.z);

        OBB.axisDir[0] = vX - box.leftBottom;
        D3DXVec3Normalize(&OBB.axisDir[0], &OBB.axisDir[0]);

        OBB.axisDir[1] = vY - box.leftBottom;
        D3DXVec3Normalize(&OBB.axisDir[1], &OBB.axisDir[1]);

        OBB.axisDir[2] = vZ - box.leftBottom;
        D3DXVec3Normalize(&OBB.axisDir[2], &OBB.axisDir[2]);

        OBB.axisLen[0] = D3DXVec3Length(&OBB.axisDir[0]) * 0.5f;
        OBB.axisLen[1] = D3DXVec3Length(&OBB.axisDir[1]) * 0.5f;
        OBB.axisLen[2] = D3DXVec3Length(&OBB.axisDir[2]) * 0.5f;

        return OBBToOBB(obb, OBB);
    }

    bool PhysicsManager::OBBToOBB(const TAG_OBB& obb1, const TAG_OBB& obb2)
    {
        ThreadSync sync;

        double c[3][3];

        double absC[3][3];

        double d[3];

        double r0, r1, r;
        int i;

        const double cutoff = 0.999999;
        bool existsParallelPair = false;

        Vector3 diff = obb1.pivot - obb2.pivot;

        for (i = 0; i < 3; ++i)
        {
            c[0][i] = D3DXVec3Dot(&obb1.axisDir[0], &obb2.axisDir[i]);
            absC[0][i] = abs(c[0][i]);
            if (absC[0][i] > cutoff)
                existsParallelPair = true;
        }

        d[0] = D3DXVec3Dot(&diff, &obb1.axisDir[0]);
        r = abs(d[0]);
        r0 = obb1.axisLen[0];
        r1 = obb2.axisLen[0] * absC[0][0] + obb2.axisLen[1] * absC[0][1] + obb2.axisLen[2] * absC[0][2];

        if (r > r0 + r1)
            return false;

        for (i = 0; i < 3; ++i)
        {
            c[1][i] = D3DXVec3Dot(&obb1.axisDir[1], &obb2.axisDir[i]);
            absC[1][i] = abs(c[1][i]);
            if (absC[1][i] > cutoff)
                existsParallelPair = true;
        }

        d[1] = D3DXVec3Dot(&diff, &obb1.axisDir[1]);
        r = abs(d[1]);
        r0 = obb1.axisLen[1];
        r1 = obb2.axisLen[0] * absC[1][0] + obb2.axisLen[1] * absC[1][1] + obb2.axisLen[2] * absC[1][2];

        if (r > r0 + r1)
            return false;

        for (i = 0; i < 3; ++i)
        {
            c[2][i] = D3DXVec3Dot(&obb1.axisDir[2], &obb2.axisDir[i]);
            absC[2][i] = abs(c[2][i]);
            if (absC[2][i] > cutoff)
                existsParallelPair = true;
        }

        d[2] = D3DXVec3Dot(&diff, &obb1.axisDir[2]);
        r = abs(d[2]);
        r0 = obb1.axisLen[2];
        r1 = obb2.axisLen[0] * absC[2][0] + obb2.axisLen[1] * absC[2][1] + obb2.axisLen[2] * absC[2][2];

        if (r > r0 + r1)
            return false;

        r = abs(D3DXVec3Dot(&diff, &obb2.axisDir[0]));
        r0 = obb1.axisLen[0] * absC[0][0] + obb1.axisLen[1] * absC[1][0] + obb1.axisLen[2] * absC[2][0];
        r1 = obb2.axisLen[0];

        if (r > r0 + r1)
            return false;

        r = abs(D3DXVec3Dot(&diff, &obb2.axisDir[1]));
        r0 = obb1.axisLen[0] * absC[0][1] + obb1.axisLen[1] * absC[1][1] + obb1.axisLen[2] * absC[2][1];
        r1 = obb2.axisLen[1];

        if (r > r0 + r1)
            return false;

        r = abs(D3DXVec3Dot(&diff, &obb2.axisDir[2]));
        r0 = obb1.axisLen[0] * absC[0][2] + obb1.axisLen[1] * absC[1][2] + obb1.axisLen[2] * absC[2][2];
        r1 = obb2.axisLen[2];

        if (r > r0 + r1)
            return false;

        if (true == existsParallelPair)
            return true;

        r = abs(d[2] * c[1][0] - d[1] * c[2][0]);
        r0 = obb1.axisLen[1] * absC[2][0] + obb1.axisLen[2] * absC[1][0];
        r1 = obb2.axisLen[1] * absC[0][2] + obb2.axisLen[2] * absC[0][1];
        if (r > r0 + r1)
            return false;

        r = abs(d[2] * c[1][1] - d[1] * c[2][1]);
        r0 = obb1.axisLen[1] * absC[2][1] + obb1.axisLen[2] * absC[1][1];
        r1 = obb2.axisLen[0] * absC[0][2] + obb2.axisLen[2] * absC[0][0];
        if (r > r0 + r1)
            return false;

        r = abs(d[2] * c[1][2] - d[1] * c[2][2]);
        r0 = obb1.axisLen[1] * absC[2][2] + obb1.axisLen[2] * absC[1][2];
        r1 = obb2.axisLen[0] * absC[0][1] + obb2.axisLen[1] * absC[0][0];
        if (r > r0 + r1)
            return false;

        r = abs(d[0] * c[2][0] - d[2] * c[0][0]);
        r0 = obb1.axisLen[0] * absC[2][0] + obb1.axisLen[2] * absC[0][0];
        r1 = obb2.axisLen[1] * absC[1][2] + obb2.axisLen[2] * absC[1][1];
        if (r > r0 + r1)
            return false;

        r = abs(d[0] * c[2][1] - d[2] * c[0][1]);
        r0 = obb1.axisLen[0] * absC[2][1] + obb1.axisLen[2] * absC[0][1];
        r1 = obb2.axisLen[0] * absC[1][2] + obb2.axisLen[2] * absC[1][0];
        if (r > r0 + r1)
            return false;

        r = abs(d[0] * c[2][2] - d[2] * c[0][2]);
        r0 = obb1.axisLen[0] * absC[2][2] + obb1.axisLen[2] * absC[0][2];
        r1 = obb2.axisLen[0] * absC[1][1] + obb2.axisLen[1] * absC[1][0];
        if (r > r0 + r1)
            return false;


        r = abs(d[1] * c[0][0] - d[0] * c[1][0]);
        r0 = obb1.axisLen[0] * absC[1][0] + obb1.axisLen[1] * absC[0][0];
        r1 = obb2.axisLen[1] * absC[2][2] + obb2.axisLen[2] * absC[2][1];
        if (r > r0 + r1)
            return false;

        r = abs(d[1] * c[0][1] - d[0] * c[1][1]);
        r0 = obb1.axisLen[0] * absC[1][1] + obb1.axisLen[1] * absC[0][1];
        r1 = obb2.axisLen[0] * absC[2][2] + obb2.axisLen[2] * absC[2][0];
        if (r > r0 + r1)
            return false;

        r = abs(d[1] * c[0][2] - d[0] * c[1][2]);
        r0 = obb1.axisLen[0] * absC[1][2] + obb1.axisLen[1] * absC[0][2];
        r1 = obb2.axisLen[0] * absC[2][1] + obb2.axisLen[1] * absC[2][0];
        if (r > r0 + r1)
            return false;

        return true;
    }
}