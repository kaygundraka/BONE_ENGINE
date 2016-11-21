#pragma once
#include "Common.h"
#include "RenderManager.h"

namespace BONE_GRAPHICS
{
	class ZFrustum
	{
		D3DXVECTOR3     m_vtx[8];       /// 프러스텀을 구성할 정점 8개
		D3DXVECTOR3     m_vPos;         /// 현재 카메라의 월드좌표
		D3DXPLANE       m_plane[6];     /// 프러스텀을 구성하는 6개의 평면

	public:
		/// 생성자
		ZFrustum();

		/// 카메라(view) * 프로젝션(projection)행렬을 입력받아 6개의 평면을 만든다.
		BOOL Make(D3DXMATRIXA16* pmatViewProj, D3DXVECTOR3 _vec3CameraPos);

		/// 한점 v가 프러스텀안에 있으면 TRUE를 반환, 아니면 FALSE를 반환한다.
		BOOL IsIn(D3DXVECTOR3* pv);

		/** 중심(v)와 반지름(radius)를 갖는 경계구(bounding sphere)가 프러스텀안에 있으면
		*  TRUE를 반환, 아니면 FALSE를 반환한다.
		*/
		BOOL IsInSphere(D3DXVECTOR3* pv, float radius);

		/// 프러스텀을 화면에 그려준다.
		BOOL Draw(LPDIRECT3DDEVICE9 pDev);

		D3DXVECTOR3 GetPos();
	};
}
