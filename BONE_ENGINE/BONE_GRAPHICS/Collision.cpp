#include "Common.h"
#include "Collision.h"
#include "LogManager.h"
using namespace BONE_GRAPHICS;

bool Collision::PointToRect(const POINT& _ptPos, const RECT& _rtRect)
{
	if (_ptPos.x > _rtRect.right)	return false;
	if (_ptPos.x < _rtRect.left)	return false;
	if (_ptPos.y < _rtRect.top)	return false;
	if (_ptPos.y > _rtRect.bottom)	return false;

	return true;
}

bool Collision::VectorToAABB(const Vector3& _vPos, const tagBox& _Box)
{
	if (_vPos.x > _Box.vMax.x)	return false;
	if (_vPos.x < _Box.vMin.x)	return false;
	if (_vPos.y > _Box.vMax.y)	return false;
	if (_vPos.y < _Box.vMin.y)	return false;
	if (_vPos.z > _Box.vMax.z)	return false;
	if (_vPos.z < _Box.vMin.z)	return false;

	return true;
}

bool Collision::AABBToAABB(const tagBox& _Box1, const tagBox& _Box2)
{
	if (_Box1.vMin.x + _Box1.vPosition.x  > _Box2.vMax.x + _Box2.vPosition.x
		|| _Box1.vMax.x + _Box1.vPosition.x < _Box2.vMin.x + _Box2.vPosition.x)
		return false;

	if (_Box1.vMin.y + _Box1.vPosition.y > _Box2.vMax.y + _Box2.vPosition.y
		|| _Box1.vMax.y + _Box1.vPosition.y < _Box2.vMin.y + _Box2.vPosition.y)
		return false;

	if (_Box1.vMin.z + _Box1.vPosition.z > _Box2.vMax.z + _Box2.vPosition.z
		|| _Box1.vMax.z + _Box1.vPosition.z < _Box2.vMin.z + _Box2.vPosition.z)
		return false;

	return true;
}

bool Collision::VectorToSphere(const Vector3& _vPos, const tagSphere& _Sphere)
{
	Vector3 RVec(Vector3(0, 0, 0));

    D3DXVec3Subtract(&RVec, &(_vPos), &(_Sphere.vPosition));

	float C = D3DXVec3Dot(&RVec, &RVec);
	float distans = _Sphere.fRadius * _Sphere.fRadius;

	if (distans > C)
		return true; // 충돌
	else
		return false; // 비충돌
}

bool Collision::SphereToSphere(const tagSphere& _Sphere1, const tagSphere& _Sphere2)
{
	Vector3 RVec(Vector3(0, 0, 0));

	//중심점간 뺄셈
	D3DXVec3Subtract(&RVec, &(_Sphere1.vPosition), &(_Sphere2.vPosition));

	//거리계산
	float C = D3DXVec3Length(&RVec);
	float distans = _Sphere1.fRadius + _Sphere2.fRadius; //반지름 합
														 //distans *= distans; //제곱

	if (distans >= C)
		return true; // 충돌
	else
		return false; // 비충돌
}
Collision::tagOBB Collision::ConvertAABBtoOBB(const tagBox& _Box, tagOBB R_OBB)
{

	R_OBB.vPosition = (_Box.vMin + _Box.vMax) / 2.0f;

	Vector3 vX(_Box.vMax.x, _Box.vMin.y, _Box.vMin.z); // x축
	Vector3 vY(_Box.vMin.x, _Box.vMax.y, _Box.vMin.z); // y축
	Vector3 vZ(_Box.vMin.x, _Box.vMin.y, _Box.vMax.z); // z축

	R_OBB.vAxisDir[0] = vX - _Box.vMin;
	D3DXVec3Normalize(&R_OBB.vAxisDir[0], &R_OBB.vAxisDir[0]);

	R_OBB.vAxisDir[1] = vY - _Box.vMin;
	D3DXVec3Normalize(&R_OBB.vAxisDir[1], &R_OBB.vAxisDir[1]);

	R_OBB.vAxisDir[2] = vZ - _Box.vMin;
	D3DXVec3Normalize(&R_OBB.vAxisDir[2], &R_OBB.vAxisDir[2]);

	R_OBB.fAxisLen[0] = D3DXVec3Length(&R_OBB.vAxisDir[0]) * 0.5f;
	R_OBB.fAxisLen[1] = D3DXVec3Length(&R_OBB.vAxisDir[1]) * 0.5f;
	R_OBB.fAxisLen[2] = D3DXVec3Length(&R_OBB.vAxisDir[2]) * 0.5f;

	return R_OBB;
}

bool Collision::AABBToOBB(const tagBox& _Box, const tagOBB& _OBB)
{
	tagOBB OBB;
	OBB.vPosition = (_Box.vMin + _Box.vMax) / 2.0f;

	Vector3 vX(_Box.vMax.x, _Box.vMin.y, _Box.vMin.z);
	Vector3 vY(_Box.vMin.x, _Box.vMax.y, _Box.vMin.z);
	Vector3 vZ(_Box.vMin.x, _Box.vMin.y, _Box.vMax.z);

	OBB.vAxisDir[0] = vX - _Box.vMin;
	D3DXVec3Normalize(&OBB.vAxisDir[0], &OBB.vAxisDir[0]);

	OBB.vAxisDir[1] = vY - _Box.vMin;
	D3DXVec3Normalize(&OBB.vAxisDir[1], &OBB.vAxisDir[1]);

	OBB.vAxisDir[2] = vZ - _Box.vMin;
	D3DXVec3Normalize(&OBB.vAxisDir[2], &OBB.vAxisDir[2]);

	OBB.fAxisLen[0] = D3DXVec3Length(&OBB.vAxisDir[0]) * 0.5f;
	OBB.fAxisLen[1] = D3DXVec3Length(&OBB.vAxisDir[1]) * 0.5f;
	OBB.fAxisLen[2] = D3DXVec3Length(&OBB.vAxisDir[2]) * 0.5f;

	return OBBToOBB(_OBB, OBB);
}

bool Collision::OBBToOBB(const tagOBB& _OBB1, const tagOBB& _OBB2)
{
	//
	double c[3][3];

	double absC[3][3];

	double d[3];

	double r0, r1, r;
	int i;

	const double cutoff = 0.999999;
	bool existsParallelPair = false;

	Vector3 diff = _OBB1.vPosition - _OBB2.vPosition;


	//
	for (i = 0; i < 3; ++i)
	{
		c[0][i] = D3DXVec3Dot(&_OBB1.vAxisDir[0], &_OBB2.vAxisDir[i]);
		absC[0][i] = abs(c[0][i]);
		if (absC[0][i] > cutoff)
			existsParallelPair = true;
	}

	d[0] = D3DXVec3Dot(&diff, &_OBB1.vAxisDir[0]);
	r = abs(d[0]);
	r0 = _OBB1.fAxisLen[0];
	r1 = _OBB2.fAxisLen[0] * absC[0][0] + _OBB2.fAxisLen[1] * absC[0][1] + _OBB2.fAxisLen[2] * absC[0][2];

	if (r > r0 + r1)
		return false;

	for (i = 0; i < 3; ++i)
	{
		c[1][i] = D3DXVec3Dot(&_OBB1.vAxisDir[1], &_OBB2.vAxisDir[i]);
		absC[1][i] = abs(c[1][i]);
		if (absC[1][i] > cutoff)
			existsParallelPair = true;
	}

	d[1] = D3DXVec3Dot(&diff, &_OBB1.vAxisDir[1]);
	r = abs(d[1]);
	r0 = _OBB1.fAxisLen[1];
	r1 = _OBB2.fAxisLen[0] * absC[1][0] + _OBB2.fAxisLen[1] * absC[1][1] + _OBB2.fAxisLen[2] * absC[1][2];

	if (r > r0 + r1)
		return false;

	for (i = 0; i < 3; ++i)
	{
		c[2][i] = D3DXVec3Dot(&_OBB1.vAxisDir[2], &_OBB2.vAxisDir[i]);
		absC[2][i] = abs(c[2][i]);
		if (absC[2][i] > cutoff)
			existsParallelPair = true;
	}

	d[2] = D3DXVec3Dot(&diff, &_OBB1.vAxisDir[2]);
	r = abs(d[2]);
	r0 = _OBB1.fAxisLen[2];
	r1 = _OBB2.fAxisLen[0] * absC[2][0] + _OBB2.fAxisLen[1] * absC[2][1] + _OBB2.fAxisLen[2] * absC[2][2];

	if (r > r0 + r1)
		return false;

	r = abs(D3DXVec3Dot(&diff, &_OBB2.vAxisDir[0]));
	r0 = _OBB1.fAxisLen[0] * absC[0][0] + _OBB1.fAxisLen[1] * absC[1][0] + _OBB1.fAxisLen[2] * absC[2][0];
	r1 = _OBB2.fAxisLen[0];

	if (r > r0 + r1)
		return false;

	r = abs(D3DXVec3Dot(&diff, &_OBB2.vAxisDir[1]));
	r0 = _OBB1.fAxisLen[0] * absC[0][1] + _OBB1.fAxisLen[1] * absC[1][1] + _OBB1.fAxisLen[2] * absC[2][1];
	r1 = _OBB2.fAxisLen[1];

	if (r > r0 + r1)
		return false;

	r = abs(D3DXVec3Dot(&diff, &_OBB2.vAxisDir[2]));
	r0 = _OBB1.fAxisLen[0] * absC[0][2] + _OBB1.fAxisLen[1] * absC[1][2] + _OBB1.fAxisLen[2] * absC[2][2];
	r1 = _OBB2.fAxisLen[2];

	if (r > r0 + r1)
		return false;

	if (true == existsParallelPair)
		return true;

	r = abs(d[2] * c[1][0] - d[1] * c[2][0]);
	r0 = _OBB1.fAxisLen[1] * absC[2][0] + _OBB1.fAxisLen[2] * absC[1][0];
	r1 = _OBB2.fAxisLen[1] * absC[0][2] + _OBB2.fAxisLen[2] * absC[0][1];
	if (r > r0 + r1)
		return false;

	r = abs(d[2] * c[1][1] - d[1] * c[2][1]);
	r0 = _OBB1.fAxisLen[1] * absC[2][1] + _OBB1.fAxisLen[2] * absC[1][1];
	r1 = _OBB2.fAxisLen[0] * absC[0][2] + _OBB2.fAxisLen[2] * absC[0][0];
	if (r > r0 + r1)
		return false;

	r = abs(d[2] * c[1][2] - d[1] * c[2][2]);
	r0 = _OBB1.fAxisLen[1] * absC[2][2] + _OBB1.fAxisLen[2] * absC[1][2];
	r1 = _OBB2.fAxisLen[0] * absC[0][1] + _OBB2.fAxisLen[1] * absC[0][0];
	if (r > r0 + r1)
		return false;

	r = abs(d[0] * c[2][0] - d[2] * c[0][0]);
	r0 = _OBB1.fAxisLen[0] * absC[2][0] + _OBB1.fAxisLen[2] * absC[0][0];
	r1 = _OBB2.fAxisLen[1] * absC[1][2] + _OBB2.fAxisLen[2] * absC[1][1];
	if (r > r0 + r1)
		return false;

	r = abs(d[0] * c[2][1] - d[2] * c[0][1]);
	r0 = _OBB1.fAxisLen[0] * absC[2][1] + _OBB1.fAxisLen[2] * absC[0][1];
	r1 = _OBB2.fAxisLen[0] * absC[1][2] + _OBB2.fAxisLen[2] * absC[1][0];
	if (r > r0 + r1)
		return false;

	r = abs(d[0] * c[2][2] - d[2] * c[0][2]);
	r0 = _OBB1.fAxisLen[0] * absC[2][2] + _OBB1.fAxisLen[2] * absC[0][2];
	r1 = _OBB2.fAxisLen[0] * absC[1][1] + _OBB2.fAxisLen[1] * absC[1][0];
	if (r > r0 + r1)
		return false;



	r = abs(d[1] * c[0][0] - d[0] * c[1][0]);
	r0 = _OBB1.fAxisLen[0] * absC[1][0] + _OBB1.fAxisLen[1] * absC[0][0];
	r1 = _OBB2.fAxisLen[1] * absC[2][2] + _OBB2.fAxisLen[2] * absC[2][1];
	if (r > r0 + r1)
		return false;



	r = abs(d[1] * c[0][1] - d[0] * c[1][1]);
	r0 = _OBB1.fAxisLen[0] * absC[1][1] + _OBB1.fAxisLen[1] * absC[0][1];
	r1 = _OBB2.fAxisLen[0] * absC[2][2] + _OBB2.fAxisLen[2] * absC[2][0];
	if (r > r0 + r1)
		return false;



	r = abs(d[1] * c[0][2] - d[0] * c[1][2]);
	r0 = _OBB1.fAxisLen[0] * absC[1][2] + _OBB1.fAxisLen[1] * absC[0][2];
	r1 = _OBB2.fAxisLen[0] * absC[2][1] + _OBB2.fAxisLen[1] * absC[2][0];
	if (r > r0 + r1)
		return false;

	return true;
}

HRESULT Collision::ComputeBoundingBox(LPD3DXMESH _lpMesh, tagBox& _Box)
{
	LPVOID pVertices(nullptr);
	_lpMesh->LockVertexBuffer(D3DLOCK_NOSYSLOCK, &pVertices);

	if (FAILED(D3DXComputeBoundingBox((Vector3*)pVertices,
		_lpMesh->GetNumVertices(),
		D3DXGetFVFVertexSize(_lpMesh->GetFVF()),
		&_Box.vMin,
		&_Box.vMax)))
	{
		LogMgr->ShowMessage(LOG_ERROR, "Compute BoundingBox Failed..");
		_lpMesh->UnlockVertexBuffer();
		return E_FAIL;
	}

	_lpMesh->UnlockVertexBuffer();

	return S_OK;
}

HRESULT Collision::ComputeBoundingSphere(LPD3DXMESH _lpMesh, tagSphere& _Sphere)
{
	LPVOID pVertices(nullptr);
	_lpMesh->LockVertexBuffer(D3DLOCK_NOSYSLOCK, &pVertices);

	if (FAILED(D3DXComputeBoundingSphere((Vector3*)pVertices,
		_lpMesh->GetNumVertices(),
		D3DXGetFVFVertexSize(_lpMesh->GetFVF()),
		&_Sphere.vPosition,
		&_Sphere.fRadius)))
	{
		LogMgr->ShowMessage(LOG_ERROR, "Compute BoundingShere Failed..");
		_lpMesh->UnlockVertexBuffer();
		return E_FAIL;
	}

	_lpMesh->UnlockVertexBuffer();

	return S_OK;
}

HRESULT Collision::ComputeBoundingSphere(LPD3DXFRAME _lpFrame, tagSphere& _Sphere)
{
	if (FAILED(D3DXFrameCalculateBoundingSphere(_lpFrame, &_Sphere.vPosition, &_Sphere.fRadius)))
	{
		LogMgr->ShowMessage(LOG_ERROR, "Compute BoundingShere Failed..");

		return E_FAIL;
	}

	return S_OK;
}

HRESULT Collision::ComputeOBB(const tagBox& _Box, tagOBB& _OBB)
{
	return S_OK;
}

BOOL Collision::GetDetectColl()
{
	return DetectCollision;
}

void Collision::SetDetectColl(BOOL dc)
{
	DetectCollision = dc;
}