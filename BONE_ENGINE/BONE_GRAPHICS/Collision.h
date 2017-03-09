#pragma once
#include "Common.h"

namespace Collision
{
	struct tagBox
	{
		Vector3	vMin;		//�� �ϴ� ��.
		Vector3	vMax;		//�� ��� ��.
		Vector3 vPosition;	// �߽���.

		tagBox(void) : vMin(Vector3(0, 0, 0)), vMax(Vector3(0, 0, 0)), vPosition(Vector3(0, 0, 0)) {}
	};

	struct tagSphere
	{
		float fRadius;		 // ������
		Vector3 vPosition;		 // �߽���

		tagSphere(void) : fRadius(0.0f), vPosition(Vector3(0, 0, 0)) {}
	};

    struct tagOBB
	{
		Vector3 vPosition;			//�߽���.
		Vector3	vAxisDir[3];		// ������ ������ ������ ���� ����
		float fAxisLen[3];		// ������ ������ ������ ����
		tagOBB(void) {
			ZeroMemory(this, sizeof(tagOBB));
		}
	};
	
    //�浹 �˻� �� ����
	static BOOL DetectCollision;
	BOOL GetDetectColl();
	void SetDetectColl(BOOL dc);
	//�� ���
	bool PointToRect(const POINT& _ptPos, const RECT& _rtRect);
	//���Ϳ� ��ȸ���ڽ�.
	bool VectorToAABB(const Vector3& _vPos, const tagBox& _Box);
	//��ȸ���ڽ� ��ȸ���ڽ�
	bool AABBToAABB(const tagBox& _Box1, const tagBox& _Box2);
	//���� ��
	bool VectorToSphere(const Vector3& _vPos, const tagSphere& _Sphere);
	//�� ��
	bool SphereToSphere(const tagSphere& _Sphere1, const tagSphere& _Sphere2);
	//��ȸ���ڽ� ȸ���ڽ� 
	bool AABBToOBB(const tagBox& _Box, const tagOBB& _OBB);
	//ȸ���ڽ� ȸ���ڽ�
	bool OBBToOBB(const tagOBB& _OBB1, const tagOBB& _OBB2);

	tagOBB ConvertAABBtoOBB(const tagBox& _Box, tagOBB R_OBB);

	//��ȸ���ڽ� ����.
	HRESULT ComputeBoundingBox(LPD3DXMESH _lpMesh, tagBox& _Box);

	//������. ���� �޽�
	HRESULT	ComputeBoundingSphere(LPD3DXMESH _lpMesh, tagSphere& _Sphere);
	//������. ���� �޽�
	HRESULT	ComputeBoundingSphere(LPD3DXFRAME _lpFrame, tagSphere& _Sphere);
	HRESULT	ComputeOBB(const tagBox& _Box, tagOBB& _OBB);
};
