#pragma once
#include "Common.h"
#include "ZFrustum.h"
#include "Vertex.h"

#define IS_IN_RANGE(value,r0,r1) (( ((r0) <= (value)) && ((value) <= (r1)) ) ? 1 : 0)

namespace BONE_GRAPHICS
{
	// QuadTree�� ���̽� Ŭ����
	class ZQuadTree
	{
		/// ����Ʈ���� �����Ǵ� 4���� �ڳʰ��� ���� �����
		enum CornerType { CORNER_TL, CORNER_TR, CORNER_BL, CORNER_BR };

		/// �̿���� ó���� �����
		enum { EDGE_UP, EDGE_DN, EDGE_LT, EDGE_RT };

		/// ����Ʈ���� �������Ұ��� ����
		enum QuadLocation {
			FRUSTUM_OUT = 0,  /// �������ҿ��� �������
			FRUSTUM_PARTIALLY_IN = 1,       /// �������ҿ� �κ�����
			FRUSTUM_COMPLETELY_IN = 2,      /// �������ҿ� ��������
			FRUSTUM_UNKNOWN = -1
		};         /// �𸣰���(^^;)

	private:
		ZQuadTree* m_pChild[4];  /// QuadTree�� 4���� �ڽĳ��

		/// Triangle Crack(Popping)�� �������ؼ� ����Ѵ�.
		ZQuadTree*      m_pParent;
		ZQuadTree*      m_pNeighbor[4];

		int m_nCenter;         /// QuadTree�� ������ ù��° ��
		int m_nCorner[4];      /// QuadTree�� ������ �ι�° ��

		BOOL  m_bCulled;        ///�������ҿ��� �ø��� ����ΰ�?
		float m_fRadius;		/// ��带 ���δ� ��豸(bounding sphere)�� ������

	private:
		/// �ڽ� ��带 �߰��Ѵ�.
		ZQuadTree* _AddChild(int nCornerTL, int nCornerTR, int nCornerBL, int nCornerBR);

		/// 4���� �ڳʰ��� �����Ѵ�.
		BOOL _SetCorners(int nCornerTL, int nCornerTR, int nCornerBL, int nCornerBR);

		/// Quadtree�� 4���� ���� Ʈ���� �κк���(subdivide)�Ѵ�.
		BOOL _SubDivide();

		/// ���� ��尡 ����� ������ ����ΰ�?
		BOOL _IsVisible() { return (m_nCorner[CORNER_TR] - m_nCorner[CORNER_TL] <= 1); }

		/// ����� �������� �ε����� �����Ѵ�.
		int _GenTriIndex(int nTris, LPVOID pIndex, VERTEX* pHeightMap, ZFrustum* pFrustum, float fLODRatio);

		/// ���� ������ �Ÿ��� ���Ѵ�.
		float _GetDistance(D3DXVECTOR3* pv1, D3DXVECTOR3* pv2);

		/// ī�޶�� ���� ������ �Ÿ����� �������� LOD���� ���Ѵ�.
		int _GetLODLevel(VERTEX* pHeightMap, D3DXVECTOR3* pCamera, float fLODRatio);

		/// �̿���带 �����.(�ﰢ�� ������ ������)
		void _BuildNeighborNode(ZQuadTree* pRoot, VERTEX* pHeightMap, int cx);

		/// 4�� ����(���,�ϴ�,����,����)�� �̿���� �ε����� ���Ѵ�.
		int  _GetNodeIndex(int ed, int cx, int& _0, int& _1, int& _2, int& _3);

		// ����Ʈ���� �˻��ؼ� 4�� �ڳʰ��� ��ġ�ϴ� ��带 ã�´�.
		ZQuadTree* _FindNode(VERTEX* pHeightMap, int _0, int _1, int _2, int _3);

		/// �����尡 �������ҿ� ���ԵǴ°�?
		int _IsInFrustum(VERTEX* pHeightMap, ZFrustum* pFrustum);

		/// _IsInFrustum()�Լ��� ����� ���� �������� �ø� ����
		void _FrustumCull(VERTEX* pHeightMap, ZFrustum* pFrustum);

		//// ����Ʈ���� �����.(Build()�Լ����� �Ҹ���)
		BOOL _BuildQuadTree(VERTEX* pHeightMap);

		/// �޸𸮿��� ����Ʈ���� �����Ѵ�.
		void _Destroy();

	public:    
		/// ���� ��Ʈ��� ������
		ZQuadTree(int cx, int cy);

		/// ���� �ڽĳ�� ������
		ZQuadTree(ZQuadTree* pParent);

		/// �Ҹ���
		~ZQuadTree();

		/// QuadTree�� �����Ѵ�.
		BOOL Build(VERTEX* pHeightMap);

		/// ���� ��尡 LOD������� ����  ����� ������ ����ΰ�?
		BOOL IsVisible(VERTEX* pHeightMap, D3DXVECTOR3* pCamera, float fLODRatio);
		
		/// �ڳ� �ε��� ���� ���´�.
		void GetCorner(int& _0, int& _1, int& _2, int& _3);

		/// �ﰢ���� �ε����� �����, ����� �ﰢ���� ������ ��ȯ�Ѵ�.
		int GenerateIndex(LPVOID pIndex, VERTEX* pHeightMap, ZFrustum* pFrustum, float fLODRatio);
	};
}