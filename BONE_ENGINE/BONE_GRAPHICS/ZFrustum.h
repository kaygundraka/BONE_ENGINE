#pragma once
#include "Common.h"
#include "RenderManager.h"

namespace BONE_GRAPHICS
{
	class ZFrustum
	{
		D3DXVECTOR3     m_vtx[8];       /// ���������� ������ ���� 8��
		D3DXVECTOR3     m_vPos;         /// ���� ī�޶��� ������ǥ
		D3DXPLANE       m_plane[6];     /// ���������� �����ϴ� 6���� ���

	public:
		/// ������
		ZFrustum();

		/// ī�޶�(view) * ��������(projection)����� �Է¹޾� 6���� ����� �����.
		bool Make(D3DXMATRIXA16* pmatViewProj, D3DXVECTOR3 _vec3CameraPos);

		/// ���� v�� �������Ҿȿ� ������ TRUE�� ��ȯ, �ƴϸ� FALSE�� ��ȯ�Ѵ�.
		bool IsIn(D3DXVECTOR3* pv);

		/** �߽�(v)�� ������(radius)�� ���� ��豸(bounding sphere)�� �������Ҿȿ� ������
		*  TRUE�� ��ȯ, �ƴϸ� FALSE�� ��ȯ�Ѵ�.
		*/
		bool IsInSphere(D3DXVECTOR3* pv, float radius);

		/// ���������� ȭ�鿡 �׷��ش�.
		bool Draw(LPDIRECT3DDEVICE9 pDev);

		D3DXVECTOR3 GetPos();
	};
}
