#pragma once
#include "Common.h"
#include "Component.h"
#include "GameObject.h"
#include "Transform3D.h"

namespace BONE_GRAPHICS
{
    typedef struct _COLLISION {
    } COLLISION;

    typedef struct _TAG_BOX : public COLLISION
    {
        Vec3 leftBottom;
        Vec3 rightTop;
        Vec3 pivot;

        _TAG_BOX() : leftBottom(Vec3(0, 0, 0)), rightTop(Vec3(0, 0, 0)), pivot(Vec3(0, 0, 0)) {}
    } TAG_BOX;

    typedef struct _TAG_SPHERE : public COLLISION
    {
        float radius;
        Vec3 pivot;

        _TAG_SPHERE() : radius(0.0f), pivot(Vec3(0, 0, 0)) {}
    } TAG_SPHERE;

    class Collision : public Component {
    public:
        enum COLLISION_TYPE { COLL_BOX, COLL_SPHERE, COLL_CONE, COLL_CYLINDER, COLL_CAPSULE };

    private:
        GameObject* object;
        Transform3D* transform;

        ConvexShape* collision;
        rp3d::CollisionBody* body;

        float radius;
        float height;
        Vec3 halfExtens;

        Color color;
        bool isShow;

        COLLISION_TYPE type;

    public:
        Collision(GameObject* attachObject);
        virtual ~Collision();
        
        bool CreateBox(Vector3 halfExtens);
        bool CreateSphere(float radius);
        bool CreateCone(float radius, float height);
        bool CreateCylinder(float radius, float height);
        bool CreateCapsule(float radius, float height);
        
        bool ComputeBoundingBox(LPD3DXMESH mesh);
        bool ComputeBoundingSphere(LPD3DXMESH mesh);
        bool ComputeBoundingSphere(LPD3DXFRAME frameMesh);

        Vec3 GetHalfExtens();
        float GetRadius();
        float GetHeight();
                
        COLLISION_TYPE GetCollisionType();

        ConvexShape* GetCollision();

        GameObject* GetGameObject();
    };
}
