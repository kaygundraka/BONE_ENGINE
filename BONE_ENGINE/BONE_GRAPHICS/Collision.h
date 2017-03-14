#pragma once
#include "Common.h"
#include "Component.h"
#include "GameObject.h"

namespace BONE_GRAPHICS
{
    typedef struct _COLLISION {
    } COLLISION;

	typedef struct _TAG_BOX : public COLLISION
	{
		Vector3	leftBottom;
		Vector3	rightTop;
		Vector3 pivot;

        _TAG_BOX() : leftBottom(Vector3(0, 0, 0)), rightTop(Vector3(0, 0, 0)), pivot(Vector3(0, 0, 0)) {}
	} TAG_BOX;

	typedef struct _TAG_SPHERE : public COLLISION
	{
        float radius;
		Vector3 pivot;

        _TAG_SPHERE() : radius(0.0f), pivot(Vector3(0, 0, 0)) {}
	} TAG_SPHERE;

    typedef struct _TAG_OBB : public COLLISION
	{
		Vector3 pivot;
		Vector3	axisDir[3];
		float axisLen[3];
		
        _TAG_OBB() {
			ZeroMemory(this, sizeof(_TAG_OBB));
		}
	} TAG_OBB;

    class Collision : public Component {
    public:
        enum COLLISION_TYPE { NONE, AABB, SPHERE, OBB };

    private:
        COLLISION* collision;
        GameObject* object;

        COLLISION_TYPE type;
        Color color;
        bool isShow;

    public:
        Collision(GameObject* attachObject);
        virtual ~Collision();

        void SetShow(bool render, D3DXCOLOR color = COLOR::WHITE);
        bool IsShow();

        void Update();
        void Render(GameObject* object);

        bool CreateAABB(Vector3 leftBottom, Vector3 rightTop, Vector3 pivot);
        bool CreateSphere(float radius, Vector3 pivot);
        bool CreateOBB(Vector3 pivot, Vector3* axisDir, float *axisLen);

        bool ComputeBoundingBox(LPD3DXMESH mesh);
        bool ComputeBoundingSphere(LPD3DXMESH mesh);
        bool ComputeBoundingSphere(LPD3DXFRAME frameMesh);

        //TAG_OBB ConvertAABBtoOBB(const TAG_BOX& box, TAG_OBB obb);

        GameObject* GetGameObject();

        COLLISION* GetCollision();
        COLLISION_TYPE GetCollisionType();

        void Reset();
    };
}
