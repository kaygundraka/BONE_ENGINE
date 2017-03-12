#pragma once
#include "Common.h"

namespace BONE_GRAPHICS
{
	typedef struct _TAG_BOX
	{
		Vector3	leftBottom;
		Vector3	rightTop;
		Vector3 pivot;

		TAG_BOX() : leftBottom(Vector3(0, 0, 0)), rightTop(Vector3(0, 0, 0)), pivot(Vector3(0, 0, 0)) {}
	} TAG_BOX;

	typedef struct _TAG_SPHERE
	{
        float radius;
		Vector3 pivot;

		TAG_SPHERE() : radius(0.0f), pivot(Vector3(0, 0, 0)) {}
	} TAG_SPHERE;

    typedef struct _TAG_OBB
	{
		Vector3 pivot;
		Vector3	axisDir[3];
		float axisLen[3];
		
        TAG_OBB() {
			ZeroMemory(this, sizeof(TAG_OBB));
		}
	} TAG_OBB;
	
    static bool DetectBONE_GRAPHICS;
	bool GetDetectColl();
	void SetDetectColl(bool dc);
	
    bool PointToRect(const Vector2& pos, const RECT& rect);
	bool VectorToAABB(const Vector3& pos, const TAG_BOX& box);
	bool AABBToAABB(const TAG_BOX& box1, const TAG_BOX& box2);
	bool VectorToSphere(const Vector3& pos, const TAG_SPHERE& sphere);
	bool SphereToSphere(const TAG_SPHERE& sphere1, const TAG_SPHERE& sphere2);
	bool AABBToOBB(const TAG_BOX& box, const TAG_OBB& obb);
	bool OBBToOBB(const TAG_OBB& obb, const TAG_OBB& obb2);

	TAG_OBB ConvertAABBtoOBB(const TAG_BOX& box, TAG_OBB obb);

	HRESULT ComputeBoundingBox(LPD3DXMESH mesh, TAG_BOX& box);
	HRESULT	ComputeBoundingSphere(LPD3DXMESH mesh, TAG_SPHERE& sphere);
	HRESULT	ComputeBoundingSphere(LPD3DXFRAME frameMesh, TAG_SPHERE& sphere);
	HRESULT	ComputeOBB(const TAG_BOX& box, TAG_OBB& obb);
}
