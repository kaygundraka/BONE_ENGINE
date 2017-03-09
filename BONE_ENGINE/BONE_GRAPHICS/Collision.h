#pragma once
#include "Common.h"

namespace Collision
{
	struct tagBox
	{
		Vector3	vMin;		//좌 하단 값.
		Vector3	vMax;		//우 상단 값.
		Vector3 vPosition;	// 중심점.

		tagBox(void) : vMin(Vector3(0, 0, 0)), vMax(Vector3(0, 0, 0)), vPosition(Vector3(0, 0, 0)) {}
	};

	struct tagSphere
	{
		float fRadius;		 // 반지름
		Vector3 vPosition;		 // 중심점

		tagSphere(void) : fRadius(0.0f), vPosition(Vector3(0, 0, 0)) {}
	};

    struct tagOBB
	{
		Vector3 vPosition;			//중심점.
		Vector3	vAxisDir[3];		// 상자의 평행한 세축의 단위 벡터
		float fAxisLen[3];		// 상자의 평행한 세축의 길이
		tagOBB(void) {
			ZeroMemory(this, sizeof(tagOBB));
		}
	};
	
    //충돌 검사 용 변수
	static BOOL DetectCollision;
	BOOL GetDetectColl();
	void SetDetectColl(BOOL dc);
	//점 평면
	bool PointToRect(const POINT& _ptPos, const RECT& _rtRect);
	//벡터와 비회전박스.
	bool VectorToAABB(const Vector3& _vPos, const tagBox& _Box);
	//비회전박스 비회전박스
	bool AABBToAABB(const tagBox& _Box1, const tagBox& _Box2);
	//벡터 구
	bool VectorToSphere(const Vector3& _vPos, const tagSphere& _Sphere);
	//구 구
	bool SphereToSphere(const tagSphere& _Sphere1, const tagSphere& _Sphere2);
	//비회전박스 회전박스 
	bool AABBToOBB(const tagBox& _Box, const tagOBB& _OBB);
	//회전박스 회전박스
	bool OBBToOBB(const tagOBB& _OBB1, const tagOBB& _OBB2);

	tagOBB ConvertAABBtoOBB(const tagBox& _Box, tagOBB R_OBB);

	//비회전박스 생성.
	HRESULT ComputeBoundingBox(LPD3DXMESH _lpMesh, tagBox& _Box);

	//구생성. 단일 메쉬
	HRESULT	ComputeBoundingSphere(LPD3DXMESH _lpMesh, tagSphere& _Sphere);
	//구생성. 계층 메쉬
	HRESULT	ComputeBoundingSphere(LPD3DXFRAME _lpFrame, tagSphere& _Sphere);
	HRESULT	ComputeOBB(const tagBox& _Box, tagOBB& _OBB);
};
