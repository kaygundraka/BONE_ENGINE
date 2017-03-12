#include "Common.h"
#include "Collision.h"
#include "LogManager.h"
using namespace BONE_GRAPHICS;

bool BONE_GRAPHICS::PointToRect(const Vector2& pos, const RECT& rect)
{
	if (pos.x > rect.right)	    return false;
	if (pos.x < rect.left)	    return false;
	if (pos.y < rect.top)	    return false;
	if (pos.y > rect.bottom)	return false;

	return true;
}

bool BONE_GRAPHICS::VectorToAABB(const Vector3& pos, const TAG_BOX& box)
{
	if (pos.x > box.rightTop.x)	    return false;
	if (pos.x < box.leftBottom.x)	return false;
	if (pos.y > box.rightTop.y)	    return false;
	if (pos.y < box.leftBottom.y)	return false;
	if (pos.z > box.rightTop.z)	    return false;
	if (pos.z < box.leftBottom.z)	return false;

	return true;
}

bool BONE_GRAPHICS::AABBToAABB(const TAG_BOX& box1, const TAG_BOX& box2)
{
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

bool BONE_GRAPHICS::VectorToSphere(const Vector3& pos, const TAG_SPHERE& sphere)
{
	Vector3 RVec(Vector3(0, 0, 0));

    D3DXVec3Subtract(&RVec, &(pos), &(sphere.pivot));

	float C = D3DXVec3Dot(&RVec, &RVec);
	float distans = sphere.radius * sphere.radius;

	if (distans > C)
		return true;
	else
		return false;
}

bool BONE_GRAPHICS::SphereToSphere(const TAG_SPHERE& sphere1, const TAG_SPHERE& sphere2)
{
	Vector3 RVec(Vector3(0, 0, 0));

	D3DXVec3Subtract(&RVec, &(sphere1.pivot), &(sphere2.pivot));

	float C = D3DXVec3Length(&RVec);
	float distans = sphere1.radius + sphere2.radius;
	
	if (distans >= C)
		return true;
	else
		return false;
}
BONE_GRAPHICS::TAG_OBB BONE_GRAPHICS::ConvertAABBtoOBB(const TAG_BOX& box, TAG_OBB obb)
{
	obb.pivot = (box.leftBottom + box.rightTop) / 2.0f;

	Vector3 X(box.rightTop.x, box.leftBottom.y, box.leftBottom.z);
	Vector3 Y(box.leftBottom.x, box.rightTop.y, box.leftBottom.z);
	Vector3 Z(box.leftBottom.x, box.leftBottom.y, box.rightTop.z);

	obb.axisDir[0] = X - box.leftBottom;
	D3DXVec3Normalize(&obb.axisDir[0], &obb.axisDir[0]);

	obb.axisDir[1] = Y - box.leftBottom;
	D3DXVec3Normalize(&obb.axisDir[1], &obb.axisDir[1]);

	obb.axisDir[2] = Z - box.leftBottom;
	D3DXVec3Normalize(&obb.axisDir[2], &obb.axisDir[2]);

	obb.axisLen[0] = D3DXVec3Length(&obb.axisDir[0]) * 0.5f;
	obb.axisLen[1] = D3DXVec3Length(&obb.axisDir[1]) * 0.5f;
	obb.axisLen[2] = D3DXVec3Length(&obb.axisDir[2]) * 0.5f;

	return obb;
}

bool BONE_GRAPHICS::AABBToOBB(const TAG_BOX& box, const TAG_OBB& obb)
{
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

bool BONE_GRAPHICS::OBBToOBB(const TAG_OBB& obb1, const TAG_OBB& obb2)
{
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

HRESULT BONE_GRAPHICS::ComputeBoundingBox(LPD3DXMESH mesh, TAG_BOX& box)
{
	LPVOID pVertices(nullptr);
	mesh->LockVertexBuffer(D3DLOCK_NOSYSLOCK, &pVertices);

	if (FAILED(D3DXComputeBoundingBox((Vector3*)pVertices,
		mesh->GetNumVertices(),
		D3DXGetFVFVertexSize(mesh->GetFVF()),
		&box.leftBottom,
		&box.rightTop)))
	{
		LogMgr->ShowMessage(LOG_ERROR, "Compute BoundingBox Failed..");
		mesh->UnlockVertexBuffer();
		return E_FAIL;
	}

	mesh->UnlockVertexBuffer();

	return S_OK;
}

HRESULT BONE_GRAPHICS::ComputeBoundingSphere(LPD3DXMESH mesh, TAG_SPHERE& sphere)
{
	LPVOID pVertices(nullptr);
	mesh->LockVertexBuffer(D3DLOCK_NOSYSLOCK, &pVertices);

	if (FAILED(D3DXComputeBoundingSphere((Vector3*)pVertices,
		mesh->GetNumVertices(),
		D3DXGetFVFVertexSize(mesh->GetFVF()),
		&sphere.pivot,
		&sphere.radius)))
	{
		LogMgr->ShowMessage(LOG_ERROR, "Compute BoundingShere Failed..");
		mesh->UnlockVertexBuffer();
		return E_FAIL;
	}

	mesh->UnlockVertexBuffer();

	return S_OK;
}

HRESULT BONE_GRAPHICS::ComputeBoundingSphere(LPD3DXFRAME _lpFrame, TAG_SPHERE& sphere)
{
	if (FAILED(D3DXFrameCalculateBoundingSphere(_lpFrame, &sphere.pivot, &sphere.radius)))
	{
		LogMgr->ShowMessage(LOG_ERROR, "Compute BoundingShere Failed..");

		return E_FAIL;
	}

	return S_OK;
}

HRESULT BONE_GRAPHICS::ComputeOBB(const TAG_BOX& box, TAG_OBB& obb)
{
	return S_OK;
}

bool BONE_GRAPHICS::GetDetectColl()
{
	return DetectBONE_GRAPHICS;
}

void BONE_GRAPHICS::SetDetectColl(bool dc)
{
	DetectBONE_GRAPHICS = dc;
}