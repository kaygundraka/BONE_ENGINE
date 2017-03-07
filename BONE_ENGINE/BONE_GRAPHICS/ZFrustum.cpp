#include "Common.h"
#include "ZFrustum.h"

const float PLANE_EPSILON = 15.0f;

namespace BONE_GRAPHICS
{
	ZFrustum::ZFrustum()
	{
		ZeroMemory(m_vtx, sizeof(m_vtx[0]) * 8);
		ZeroMemory(m_plane, sizeof(m_plane[0]) * 6);
	}

	/// 카메라(view) * 프로젝션(projection)행렬을 입력받아 6개의 평면을 만든다.
	BOOL ZFrustum::Make(D3DXMATRIXA16* pmatViewProj, D3DXVECTOR3 _vec3CameraPos)
	{
		int i;
		D3DXMATRIXA16 matInv;

		// 투영행렬까지 거치면 모든 3차원 월드좌표의 점은 (-1, -1, 0) ~ (1, 1, 1)사이의 값으로 바뀐다.
		// m_vtx에 이 동치공간의 경계값을 넣어둔다.
		m_vtx[0].x = -1.0f;     m_vtx[0].y = -1.0f;     m_vtx[0].z = 0.0f;
		m_vtx[1].x = 1.0f;		m_vtx[1].y = -1.0f;     m_vtx[1].z = 0.0f;
		m_vtx[2].x = 1.0f;		m_vtx[2].y = -1.0f;     m_vtx[2].z = 1.0f;
		m_vtx[3].x = -1.0f;     m_vtx[3].y = -1.0f;     m_vtx[3].z = 1.0f;
		m_vtx[4].x = -1.0f;     m_vtx[4].y = 1.0f;		m_vtx[4].z = 0.0f;
		m_vtx[5].x = 1.0f;		m_vtx[5].y = 1.0f;		m_vtx[5].z = 0.0f;
		m_vtx[6].x = 1.0f;		m_vtx[6].y = 1.0f;		m_vtx[6].z = 1.0f;
		m_vtx[7].x = -1.0f;     m_vtx[7].y = 1.0f;		m_vtx[7].z = 1.0f;

		// view * proj의 역행렬을 구한다.
		D3DXMatrixInverse(&matInv, nullptr, pmatViewProj);

		// Vertex_최종 = Vertex_local * Matrix_world * Matrix_view * Matrix_Proj 인데,
		// Vertex_world = Vertex_local * Matrix_world이므로,
		// Vertex_최종 = Vertex_world * Matrix_view * Matrix_Proj 이다.
		// Vertex_최종 = Vertex_world * ( Matrix_view * Matrix_Proj ) 에서
		// 역행렬( Matrix_view * Matrix_Proj )^-1를 양변에 곱하면
		// Vertex_최종 * 역행렬( Matrix_view * Matrix_Proj )^-1 = Vertex_World 가 된다.
		// 그러므로, m_vtx * matInv = Vertex_world가 되어, 월드좌표계의 프러스텀 좌표를 얻을 수 있다.
		for (i = 0; i < 8; i++)
			D3DXVec3TransformCoord(&m_vtx[i], &m_vtx[i], &matInv);

		// 0번과 5번은 프러스텀중 near평면의 좌측상단과 우측하단이므로, 둘의 좌표를 더해서 2로 나누면
		// 카메라의 좌표를 얻을 수 있다.(정확히 일치하는 것은 아니다.)
		m_vPos = _vec3CameraPos;//(m_vtx[0] + m_vtx[5]) / 2.0f;

		// 얻어진 월드좌표로 프러스텀 평면을 만든다
		// 벡터가 프러스텀 안쪽에서 바깥쪽으로 나가는 평면들이다.
		D3DXPlaneFromPoints(&m_plane[3], m_vtx + 2, m_vtx + 6, m_vtx + 7);    // 원 평면(far)
		D3DXPlaneFromPoints(&m_plane[4], m_vtx, m_vtx + 3, m_vtx + 7);    // 좌 평면(left)
		D3DXPlaneFromPoints(&m_plane[5], m_vtx + 1, m_vtx + 5, m_vtx + 6);    // 우 평면(right)

		return TRUE;
	}

	/// 한점 v가 프러스텀안에 있으면 TRUE를 반환, 아니면 FALSE를 반환한다.
	BOOL ZFrustum::IsIn(D3DXVECTOR3* pv)
	{
		float fDist;

		// 현재는 left, right, far plane만 적용한다.
		fDist = D3DXPlaneDotCoord(&m_plane[3], pv);
		if (fDist > PLANE_EPSILON) return FALSE;       // plane의 normal벡터가 far로 향하고 있으므로 양수이면 프러스텀의 바깥쪽

		fDist = D3DXPlaneDotCoord(&m_plane[4], pv);
		if (fDist > PLANE_EPSILON) return FALSE;       // plane의 normal벡터가 left로 향하고 있으므로 양수이면 프러스텀의 왼쪽

		fDist = D3DXPlaneDotCoord(&m_plane[5], pv);
		if (fDist > PLANE_EPSILON) return FALSE;       // plane의 normal벡터가 right로 향하고 있으므로 양수이면 프러스텀의 오른쪽

		return TRUE;
	}

	/** 중심(v)와 반지름(radius)를 갖는 경계구(bounding sphere)가 프러스텀안에 있으면
	   TRUE를 반환, 아니면 FALSE를 반환한다. */
	BOOL ZFrustum::IsInSphere(D3DXVECTOR3* pv, float radius)
	{
		float fDist;

		fDist = D3DXPlaneDotCoord(&m_plane[3], pv);
		
		// 평면과 중심점의 거리가 반지름보다 크면 프러스텀에 없음
		if (fDist > (radius + PLANE_EPSILON)) return FALSE;
		
		fDist = D3DXPlaneDotCoord(&m_plane[4], pv);
		
		// 평면과 중심점의 거리가 반지름보다 크면 프러스텀에 없음
		if (fDist > (radius + PLANE_EPSILON)) return FALSE;
		
		fDist = D3DXPlaneDotCoord(&m_plane[5], pv);
		
		// 평면과 중심점의 거리가 반지름보다 크면 프러스텀에 없음
		if (fDist > (radius + PLANE_EPSILON)) return FALSE;

		return TRUE;
	}

	/// 프러스텀을 화면에 그려준다.
	BOOL ZFrustum::Draw(LPDIRECT3DDEVICE9 pDev)
	{
		WORD index[] = { 0, 1, 2,
			0, 2, 3,
			4, 7, 6,
			4, 6, 5,
			1, 5, 6,
			1, 6, 2,
			0, 3, 7,
			0, 7, 4,
			0, 4, 5,
			0, 5, 1,
			3, 7, 6,
			3, 6, 2 };

		D3DMATERIAL9 mtrl;
		ZeroMemory(&mtrl, sizeof(D3DMATERIAL9));

		typedef struct tagVTX
		{
			D3DXVECTOR3     p;
		} VTX;

		VTX             vtx[8];

		for (int i = 0; i < 8; i++)
			vtx[i].p = m_vtx[i];

		pDev->SetFVF(D3DFVF_XYZ);
		pDev->SetStreamSource(0, nullptr, 0, sizeof(VTX));
		pDev->SetTexture(0, nullptr);
		pDev->SetIndices(0);
		pDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
		pDev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		pDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		pDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

		// 파란색으로 상,하 평면을 그린다.
		pDev->SetRenderState(D3DRS_LIGHTING, TRUE);
		ZeroMemory(&mtrl, sizeof(D3DMATERIAL9));
		mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
		pDev->SetMaterial(&mtrl);
		pDev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 8, 4, index, D3DFMT_INDEX16, vtx, sizeof(vtx[0]));

		// 녹색으로 좌,우 평면을 그린다.
		ZeroMemory(&mtrl, sizeof(D3DMATERIAL9));
		mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
		pDev->SetMaterial(&mtrl);
		pDev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 8, 4, index + 4 * 3, D3DFMT_INDEX16, vtx, sizeof(vtx[0]));

		// 붉은색으로 원,근 평면을 그린다.
		ZeroMemory(&mtrl, sizeof(D3DMATERIAL9));
		mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
		pDev->SetMaterial(&mtrl);
		pDev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 8, 4, index + 8 * 3, D3DFMT_INDEX16, vtx, sizeof(vtx[0]));

		pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		pDev->SetRenderState(D3DRS_LIGHTING, FALSE);

		return TRUE;
	}

	D3DXVECTOR3 ZFrustum::GetPos()
	{
		return m_vPos;
	}
}