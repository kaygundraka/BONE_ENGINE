#include "Common.h"
#include "ZQuadTree.h"

namespace BONE_GRAPHICS
{
	/// ���� ��Ʈ��� ������
	ZQuadTree::ZQuadTree(int cx, int cy)
	{
		int i;
		m_nCenter = 0;

		for (i = 0; i < 4; i++)
		{
			m_pChild[i] = nullptr;
		}

		m_nCorner[CORNER_TL] = 0;
		m_nCorner[CORNER_TR] = cx - 1;
		m_nCorner[CORNER_BL] = cx * (cy - 1);
		m_nCorner[CORNER_BR] = cx * cy - 1;
		m_nCenter = (m_nCorner[CORNER_TL] + m_nCorner[CORNER_TR] + m_nCorner[CORNER_BL] + m_nCorner[CORNER_BR]) / 4;

		m_fRadius = 0.0f;
		m_bCulled = FALSE;
		/// �ø��� FALSE�� �ʱ�ȭ�Ѵ�. �⺻������ ��� ���� ������
		/// �ϵ��� ������Ű�� �ʿ���� �κ��� ���߿� ©�󳽴�
	}

	/// ���� �ڽĳ�� ������
	ZQuadTree::ZQuadTree(ZQuadTree* pParent)
	{
		int i;
		m_nCenter = 0;
		for (i = 0; i < 4; i++)
		{
			m_pChild[i] = nullptr;
			m_nCorner[i] = 0;
		}

		m_bCulled = FALSE;
		m_fRadius = 0.0f;
	}

	/// �Ҹ���
	ZQuadTree::~ZQuadTree()
	{
		_Destroy();
	}

	/// �޸𸮿��� ����Ʈ���� �����Ѵ�.
	void ZQuadTree::_Destroy()
	{
		for (int i = 0; i < 4; i++) delete m_pChild[i];
	}

	/// 4���� �ڳʰ��� �����Ѵ�.
	BOOL ZQuadTree::_SetCorners(int nCornerTL, int nCornerTR, int nCornerBL, int nCornerBR)
	{
		m_nCorner[CORNER_TL] = nCornerTL;
		m_nCorner[CORNER_TR] = nCornerTR;
		m_nCorner[CORNER_BL] = nCornerBL;
		m_nCorner[CORNER_BR] = nCornerBR;
		m_nCenter = (m_nCorner[CORNER_TL] + m_nCorner[CORNER_TR] + m_nCorner[CORNER_BL] + m_nCorner[CORNER_BR]) / 4;
		return TRUE;
	}

	/// ���� ������ �Ÿ��� ���Ѵ�.
	float ZQuadTree::_GetDistance(D3DXVECTOR3* pv1, D3DXVECTOR3* pv2)
	{
		return D3DXVec3Length(&(*pv2 - *pv1));
	}

	/// ī�޶�� ���� ������ �Ÿ����� �������� LOD���� ���Ѵ�.
	int ZQuadTree::_GetLODLevel(VERTEX* pHeightMap, D3DXVECTOR3* pCamera, float fLODRatio)
	{
		float d = _GetDistance((D3DXVECTOR3*)(pHeightMap + m_nCenter), pCamera);
		return max((int)(d * fLODRatio), 1);
	}

	/// ���� ��尡 LOD������� ����  ����� ������ ����ΰ�?
	BOOL ZQuadTree::IsVisible(VERTEX* pHeightMap, D3DXVECTOR3* pCamera, float fLODRatio)
	{
		return ((m_nCorner[CORNER_TR] - m_nCorner[CORNER_TL]) <= _GetLODLevel(pHeightMap, pCamera, fLODRatio));
	}

	/// �ڳ� �ε��� ���� ���´�.
	void ZQuadTree::GetCorner(int& _0, int& _1, int& _2, int& _3)
	{
		_0 = m_nCorner[0];
		_1 = m_nCorner[1]; _2 = m_nCorner[2]; _3 = m_nCorner[3];
	}

	/// �ڽ� ��带 �߰��Ѵ�.
	ZQuadTree* ZQuadTree::_AddChild(int nCornerTL, int nCornerTR, int
		nCornerBL, int nCornerBR)
	{
		ZQuadTree*      pChild;

		pChild = new ZQuadTree(this);
		pChild->_SetCorners(nCornerTL, nCornerTR, nCornerBL, nCornerBR);

		return pChild;
	}

	/// Quadtree�� 4���� �ڽ� Ʈ���� �κк���(subdivide)�Ѵ�.
	BOOL ZQuadTree::_SubDivide()
	{
		int nTopEdgeCenter;
		int nBottomEdgeCenter;
		int nLeftEdgeCenter;
		int nRightEdgeCenter;
		int nCentralPoint;

		// ��ܺ� ���
		nTopEdgeCenter = (m_nCorner[CORNER_TL] + m_nCorner[CORNER_TR]) / 2;
		// �ϴܺ� ���
		nBottomEdgeCenter = (m_nCorner[CORNER_BL] + m_nCorner[CORNER_BR]) / 2;
		// ������ ���
		nLeftEdgeCenter = (m_nCorner[CORNER_TL] + m_nCorner[CORNER_BL]) / 2;
		// ������ ���
		nRightEdgeCenter = (m_nCorner[CORNER_TR] + m_nCorner[CORNER_BR]) / 2;
		// �Ѱ��
		nCentralPoint = (m_nCorner[CORNER_TL] + m_nCorner[CORNER_TR] +
			m_nCorner[CORNER_BL] + m_nCorner[CORNER_BR]) / 4;

		// ���̻� ������ �Ұ����Ѱ�? �׷��ٸ� SubDivide() ����
		if ((m_nCorner[CORNER_TR] - m_nCorner[CORNER_TL]) <= 1)
		{
			return FALSE;
		}

		// 4���� �ڽĳ�� �߰�
		m_pChild[CORNER_TL] = _AddChild(m_nCorner[CORNER_TL],
			nTopEdgeCenter, nLeftEdgeCenter, nCentralPoint);
		m_pChild[CORNER_TR] = _AddChild(nTopEdgeCenter, m_nCorner
			[CORNER_TR], nCentralPoint, nRightEdgeCenter);
		m_pChild[CORNER_BL] = _AddChild(nLeftEdgeCenter, nCentralPoint,
			m_nCorner[CORNER_BL], nBottomEdgeCenter);
		m_pChild[CORNER_BR] = _AddChild(nCentralPoint, nRightEdgeCenter,
			nBottomEdgeCenter, m_nCorner[CORNER_BR]);

		return TRUE;
	}

	/// ����� �������� �ε����� �����Ѵ�.
	int ZQuadTree::_GenTriIndex(int nTris, LPVOID pIndex, VERTEX* pHeightMap, ZFrustum* pFrustum, float fLODRatio)
	{
		// �ø��� ����� �׳� ����
		if (m_bCulled)
		{
			m_bCulled = FALSE;
			/// ������ �ٽ� ������ �ϴ� �κ��� �˻��ϱ� ���ؼ� �⺻����
			/// ���� �������´�
			return nTris;
		}

		LPWORD p = ((LPWORD)pIndex) + nTris * 3;

		if (IsVisible(pHeightMap, &pFrustum->GetPos(), fLODRatio))   ///���̻� ���������� �ڽ��� ������ �ε����߰�
		{
			// ���� ������ ����� �κк���(subdivide)�� �Ұ����ϹǷ�
			// �׳� ����ϰ� �����Ѵ�.
			if (m_nCorner[CORNER_TR] - m_nCorner[CORNER_TL] <= 1)
			{
				*p++ = m_nCorner[0];
				*p++ = m_nCorner[1];
				*p++ = m_nCorner[2];
				nTris++;
				*p++ = m_nCorner[2];
				*p++ = m_nCorner[1];
				*p++ = m_nCorner[3];
				nTris++;
				return nTris;
			}

			BOOL    b[4];
			// ��� �̿����(neightbor node)�� ��°����Ѱ�?
			if (m_pNeighbor[EDGE_UP]) b[EDGE_UP] = m_pNeighbor[EDGE_UP]->IsVisible(pHeightMap, &pFrustum->GetPos(), fLODRatio);
			// �ϴ� �̿����(neightbor node)�� ��°����Ѱ�?
			if (m_pNeighbor[EDGE_DN]) b[EDGE_DN] = m_pNeighbor[EDGE_DN]->IsVisible(pHeightMap, &pFrustum->GetPos(), fLODRatio);
			// ���� �̿����(neightbor node)�� ��°����Ѱ�?
			if (m_pNeighbor[EDGE_LT]) b[EDGE_LT] = m_pNeighbor[EDGE_LT]->IsVisible(pHeightMap, &pFrustum->GetPos(), fLODRatio);
			// ���� �̿����(neightbor node)�� ��°����Ѱ�?
			if (m_pNeighbor[EDGE_RT]) b[EDGE_RT] = m_pNeighbor[EDGE_RT]->IsVisible(pHeightMap, &pFrustum->GetPos(), fLODRatio);

			// �̿������� ��δ� ��°����ϴٸ� ������� �̿������� ���� LOD
			// �̹Ƿ� �κк����� �ʿ����.
			if (b[EDGE_UP] && b[EDGE_DN] && b[EDGE_LT] && b[EDGE_RT])
			{
				*p++ = m_nCorner[0];
				*p++ = m_nCorner[1];
				*p++ = m_nCorner[2];
				nTris++;
				*p++ = m_nCorner[2];
				*p++ = m_nCorner[1];
				*p++ = m_nCorner[3];
				nTris++;
				return nTris;
			}

			int  n;

			if (!b[EDGE_UP]) // ��� �κк����� �ʿ��Ѱ�?
			{
				n = (m_nCorner[CORNER_TL] + m_nCorner[CORNER_TR]) / 2;
				*p++ = m_nCenter; *p++ = m_nCorner[CORNER_TL]; *p++ = n; nTris++;
				*p++ = m_nCenter; *p++ = n; *p++ = m_nCorner[CORNER_TR]; nTris++;
			}
			else    // ��� �κк����� �ʿ���� ���
			{
				*p++ = m_nCenter; *p++ = m_nCorner[CORNER_TL];
				*p++ = m_nCorner[CORNER_TR]; nTris++;
			}

			if (!b[EDGE_DN]) // �ϴ� �κк����� �ʿ��Ѱ�?
			{
				n = (m_nCorner[CORNER_BL] + m_nCorner[CORNER_BR]) / 2;
				*p++ = m_nCenter; *p++ = m_nCorner[CORNER_BR]; *p++ = n; nTris++;
				*p++ = m_nCenter; *p++ = n; *p++ = m_nCorner[CORNER_BL]; nTris++;
			}
			else    // �ϴ� �κк����� �ʿ���� ���
			{
				*p++ = m_nCenter; *p++ = m_nCorner[CORNER_BR];
				*p++ = m_nCorner[CORNER_BL]; nTris++;
			}

			if (!b[EDGE_LT]) // ���� �κк����� �ʿ��Ѱ�?
			{
				n = (m_nCorner[CORNER_TL] + m_nCorner[CORNER_BL]) / 2;
				*p++ = m_nCenter; *p++ = m_nCorner[CORNER_BL]; *p++ = n; nTris++;
				*p++ = m_nCenter; *p++ = n; *p++ = m_nCorner[CORNER_TL]; nTris++;
			}
			else    // ���� �κк����� �ʿ���� ���
			{
				*p++ = m_nCenter; *p++ = m_nCorner[CORNER_BL];
				*p++ = m_nCorner[CORNER_TL]; nTris++;
			}

			if (!b[EDGE_RT]) // ���� �κк����� �ʿ��Ѱ�?
			{
				n = (m_nCorner[CORNER_TR] + m_nCorner[CORNER_BR]) / 2;
				*p++ = m_nCenter; *p++ = m_nCorner[CORNER_TR]; *p++ = n; nTris++;
				*p++ = m_nCenter; *p++ = n; *p++ = m_nCorner[CORNER_BR]; nTris++;
			}
			else    // ���� �κк����� �ʿ���� ���
			{
				*p++ = m_nCenter; *p++ = m_nCorner[CORNER_TR];
				*p++ = m_nCorner[CORNER_BR]; nTris++;
			}

			return nTris;
			// �� ��� �Ʒ��� �ڽĳ��� Ž���� �ʿ�����Ƿ� ����!

		} // ������带 ������� �Ⱦ�����

		  // �ڽ� ���� �˻�
		if (m_pChild[CORNER_TL]) nTris = m_pChild[CORNER_TL]->_GenTriIndex(nTris, pIndex, pHeightMap, pFrustum, fLODRatio);
		if (m_pChild[CORNER_TR]) nTris = m_pChild[CORNER_TR]->_GenTriIndex(nTris, pIndex, pHeightMap, pFrustum, fLODRatio);
		if (m_pChild[CORNER_BL]) nTris = m_pChild[CORNER_BL]->_GenTriIndex(nTris, pIndex, pHeightMap, pFrustum, fLODRatio);
		if (m_pChild[CORNER_BR]) nTris = m_pChild[CORNER_BR]->_GenTriIndex(nTris, pIndex, pHeightMap, pFrustum, fLODRatio);

		return nTris;
	}

	int ZQuadTree::_IsInFrustum(VERTEX* pHeightMap, ZFrustum* pFrustum)
	{
		BOOL    b[4];
		BOOL    bInSphere;

		// ��豸�ȿ� �ִ°�?
		//if( m_fRadius == 0.0f )
		//       g_pLog->Log( "Index:[%d], Radius:[%f]",m_nCenter, m_fRadius );

		/// �߽�(pHeightMap+m_nCenter)�� ������(m_fRadius) �� ���� ��豸��
		/// ö��ü�� �����ϴ��� �˻�
		bInSphere = pFrustum->IsInSphere(
			(D3DXVECTOR3*)(pHeightMap + m_nCenter), m_fRadius);

		// ��豸 �ȿ� ������ �������� �������� �׽�Ʈ ����
		if (!bInSphere) return FRUSTUM_OUT;
		// ����Ʈ���� 4���� ������ ����ü �ȿ� �ִ����� �˻��Ѵ�
		// IsIn()���� v�� �������Ҿȿ� ������ true ����
		b[0] = pFrustum->IsIn((D3DXVECTOR3*)(pHeightMap + m_nCorner[0]));
		b[1] = pFrustum->IsIn((D3DXVECTOR3*)(pHeightMap + m_nCorner[1]));
		b[2] = pFrustum->IsIn((D3DXVECTOR3*)(pHeightMap + m_nCorner[2]));
		b[3] = pFrustum->IsIn((D3DXVECTOR3*)(pHeightMap + m_nCorner[3]));

		/// 4����� �������� �ȿ� ������ �̹� ��� ������ ���ԵǾ� �ֱ�
		/// ������ ����Ʈ���� �˻��� �ʿ� ����
		if ((b[0] + b[1] + b[2] + b[3]) == 4)
			return FRUSTUM_COMPLETELY_IN;


		// �Ϻκ��� �������ҿ� �ִ� ���
		return FRUSTUM_PARTIALLY_IN;
	}

	// _IsInFrustum()�Լ��� ����� ���� �������� �ø� ����
	void    ZQuadTree::_FrustumCull(VERTEX* pHeightMap, ZFrustum* pFrustum)
	{
		int ret;

		ret = _IsInFrustum(pHeightMap, pFrustum);
		switch (ret)
		{
		case FRUSTUM_COMPLETELY_IN:
			// �������ҿ� ��������, ������� �˻� �ʿ����
			m_bCulled = FALSE;
			return;
		case FRUSTUM_PARTIALLY_IN:
			// �������ҿ� �Ϻ�����, ������� �˻� �ʿ���
			m_bCulled = FALSE;
			break;
		case FRUSTUM_OUT:
			// �������ҿ��� �������, ������� �˻� �ʿ����
			m_bCulled = TRUE;
			return;
		}
		///�ڽĳ�带 �˻��Ѵ�
		if (m_pChild[0])
			m_pChild[0]->_FrustumCull(pHeightMap, pFrustum);
		if (m_pChild[1])
			m_pChild[1]->_FrustumCull(pHeightMap, pFrustum);
		if (m_pChild[2])
			m_pChild[2]->_FrustumCull(pHeightMap, pFrustum);
		if (m_pChild[3])
			m_pChild[3]->_FrustumCull(pHeightMap, pFrustum);
	}

	// 4�� ����(���,�ϴ�,����,����)�� �̿���� �ε����� ���Ѵ�.
	int  ZQuadTree::_GetNodeIndex(int ed, int cx, int& _0, int& _1, int& _2, int& _3)
	{
		int  n, _a, _b, _c, _d, gap;
		_a = _0;
		_b = _1;
		_c = _2;
		_d = _3;
		gap = _b - _a;      // ���� ����� �¿�����

		switch (ed)
		{
		case EDGE_UP:   // ���� ���� �̿������ �ε���
			_0 = _a - cx * gap;
			_1 = _b - cx * gap;
			_2 = _a;
			_3 = _b;
			break;
		case EDGE_DN:   // �Ʒ� ���� �̿������ �ε���
			_0 = _c;
			_1 = _d;
			_2 = _c + cx * gap;
			_3 = _d + cx * gap;
			break;
		case EDGE_LT:   // ���� ���� �̿������ �ε���
			_0 = _a - gap;
			_1 = _a;
			_2 = _c - gap;
			_3 = _c;
			break;
		case EDGE_RT:   // ���� ���� �̿������ �ε���
			_0 = _b;
			_1 = _b + gap;
			_2 = _d;
			_3 = _d + gap;
			break;
		}

		n = (_0 + _1 + _2 + _3) / 4;  // ��� �ε���
		if (!IS_IN_RANGE(n, 0, cx * cx - 1)) return -1;

		return n;
	}

	BOOL IsInRect(RECT* rc, POINT pt)
	{
		if (rc->left <= pt.x && rc->right >= pt.x)
		{
			if (rc->top <= pt.y && rc->bottom >= pt.y)
			{
				return true;
			}
		}

		return false;
	}

	// ����Ʈ���� �˻��ؼ� 4�� �ڳʰ��� ��ġ�ϴ� ��带 ã�´�.
	ZQuadTree* ZQuadTree::_FindNode(VERTEX* pHeightMap, int _0, int _1, int _2, int _3)
	{
		ZQuadTree*      p = nullptr;

		// ��ġ�ϴ� ����� ��尪�� ����
		if ((m_nCorner[0] == _0) && (m_nCorner[1] == _1) && (m_nCorner[2] == _2)
			&& (m_nCorner[3] == _3))  return this;

		// �ڽ� ��尡 �ְ�?
		if (m_pChild[0])
		{
			RECT    rc;
			POINT   pt;
			int n = (_0 + _1 + _2 + _3) / 4;

			// ���� �ʻ󿡼��� ��ġ
			pt.x = (int)pHeightMap[n].p.x;
			pt.y = (int)pHeightMap[n].p.z;

			// 4���� �ڳʰ��� �������� �ڽĳ���� �� ���������� ��´�.
			SetRect(&rc, (int)pHeightMap[m_pChild[0]->m_nCorner[CORNER_TL]].p.x,
				(int)pHeightMap[m_pChild[0]->m_nCorner[CORNER_TL]].p.z,
				(int)pHeightMap[m_pChild[0]->m_nCorner[CORNER_BR]].p.x,
				(int)pHeightMap[m_pChild[0]->m_nCorner[CORNER_BR]].p.z);

			// pt���� ���������ȿ� �ִٸ� �ڽĳ��� ����.
			if (IsInRect(&rc, pt))
				return m_pChild[0]->_FindNode(pHeightMap, _0, _1, _2, _3);

			/// ������ �ڽĵ� �˻�
			SetRect(&rc, (int)pHeightMap[m_pChild[1]->m_nCorner[CORNER_TL]].p.x,
				(int)pHeightMap[m_pChild[1]->m_nCorner[CORNER_TL]].p.z,
				(int)pHeightMap[m_pChild[1]->m_nCorner[CORNER_BR]].p.x,
				(int)pHeightMap[m_pChild[1]->m_nCorner[CORNER_BR]].p.z);

			if (IsInRect(&rc, pt))
				return m_pChild[1]->_FindNode(pHeightMap, _0, _1, _2, _3);

			SetRect(&rc, (int)pHeightMap[m_pChild[2]->m_nCorner[CORNER_TL]].p.x,
				(int)pHeightMap[m_pChild[2]->m_nCorner[CORNER_TL]].p.z,
				(int)pHeightMap[m_pChild[2]->m_nCorner[CORNER_BR]].p.x,
				(int)pHeightMap[m_pChild[2]->m_nCorner[CORNER_BR]].p.z);

			if (IsInRect(&rc, pt))
				return m_pChild[2]->_FindNode(pHeightMap, _0, _1, _2, _3);

			SetRect(&rc, (int)pHeightMap[m_pChild[3]->m_nCorner[CORNER_TL]].p.x,
				(int)pHeightMap[m_pChild[3]->m_nCorner[CORNER_TL]].p.z,
				(int)pHeightMap[m_pChild[3]->m_nCorner[CORNER_BR]].p.x,
				(int)pHeightMap[m_pChild[3]->m_nCorner[CORNER_BR]].p.z);

			if (IsInRect(&rc, pt))
				return m_pChild[3]->_FindNode(pHeightMap, _0, _1, _2, _3);

		} ///�ڽİ˻��� , �ڽĿ� ������ ���� ����

		return nullptr;
	}

	// �̿���带 �����.(�ﰢ�� ������ ������)
	void ZQuadTree::_BuildNeighborNode(ZQuadTree* pRoot, VERTEX* pHeightMap, int cx)
	{
		int n;
		int _0, _1, _2, _3;

		for (int i = 0; i<4; i++)
		{
			_0 = m_nCorner[0];
			_1 = m_nCorner[1];
			_2 = m_nCorner[2];
			_3 = m_nCorner[3];
			// �̿������ 4�� �ڳʰ��� ��´�.
			n = _GetNodeIndex(i, cx, _0, _1, _2, _3);
			// �ڳʰ����� �̿������ �����͸� ���´�.
			if (n >= 0) m_pNeighbor[i] = pRoot->_FindNode
				(pHeightMap, _0, _1, _2, _3);
		}

		// �ڽĳ��� ���ȣ��
		if (m_pChild[0])
		{
			m_pChild[0]->_BuildNeighborNode(pRoot, pHeightMap, cx);
			m_pChild[1]->_BuildNeighborNode(pRoot, pHeightMap, cx);
			m_pChild[2]->_BuildNeighborNode(pRoot, pHeightMap, cx);
			m_pChild[3]->_BuildNeighborNode(pRoot, pHeightMap, cx);
		}
	}

	// ����Ʈ���� �����.(Build()�Լ����� �Ҹ���)
	BOOL ZQuadTree::_BuildQuadTree(VERTEX* pHeightMap)
	{
		if (_SubDivide())
		{
			// ������ܰ�, ���� �ϴ��� �Ÿ��� ���Ѵ�.
			D3DXVECTOR3 v = *((D3DXVECTOR3*)(pHeightMap + m_nCorner[CORNER_TL]))
				- *((D3DXVECTOR3*)(pHeightMap + m_nCorner[CORNER_BR]));
			// v�� �Ÿ����� �� ��带 ���δ� ��豸�� �����̹Ƿ�,
			// 2�� ������ �������� ���Ѵ�.
			m_fRadius = D3DXVec3Length(&v) / 2.0f;
			m_pChild[CORNER_TL]->_BuildQuadTree(pHeightMap);
			m_pChild[CORNER_TR]->_BuildQuadTree(pHeightMap);
			m_pChild[CORNER_BL]->_BuildQuadTree(pHeightMap);
			m_pChild[CORNER_BR]->_BuildQuadTree(pHeightMap);
		}
		return TRUE;
	}

	/// QuadTree�� �����Ѵ�.
	BOOL ZQuadTree::Build(VERTEX* pHeightMap)
	{
		// ����Ʈ�� ����
		_BuildQuadTree(pHeightMap);
		// �̿���� ����
		_BuildNeighborNode(this, pHeightMap, m_nCorner[CORNER_TR] + 1);
		
		return TRUE;
	}

	// �ﰢ���� �ε����� �����, ����� �ﰢ���� ������ ��ȯ�Ѵ�.
	int ZQuadTree::GenerateIndex(LPVOID pIndex, VERTEX* pHeightMap, ZFrustum* pFrustum, float fLODRatio)
	{
		// ���� �������� �ø��� �ؼ� �ø��� ������ �����Ѵ�.
		_FrustumCull(pHeightMap, pFrustum);
		// ����� �������� �ε����� �����ѵ�, �������� ������ �����Ѵ�.
		return _GenTriIndex(0, pIndex, pHeightMap, pFrustum, fLODRatio);
	}
}