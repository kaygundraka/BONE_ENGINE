#pragma once
#include "Common.h"
#include "ZFrustum.h"
#include "Vertex.h"

#define IS_IN_RANGE(value,r0,r1) (( ((r0) <= (value)) && ((value) <= (r1)) ) ? 1 : 0)

namespace BONE_GRAPHICS
{
	// QuadTree의 베이스 클래스
	class ZQuadTree
	{
		/// 쿼드트리에 보관되는 4개의 코너값에 대한 상수값
		enum CornerType { CORNER_TL, CORNER_TR, CORNER_BL, CORNER_BR };

		/// 이웃노드 처리용 상수값
		enum { EDGE_UP, EDGE_DN, EDGE_LT, EDGE_RT };

		/// 쿼드트리와 프러스텀간의 관계
		enum QuadLocation {
			FRUSTUM_OUT = 0,  /// 프러스텀에서 완전벗어남
			FRUSTUM_PARTIALLY_IN = 1,       /// 프러스텀에 부분포함
			FRUSTUM_COMPLETELY_IN = 2,      /// 프러스텀에 완전포함
			FRUSTUM_UNKNOWN = -1
		};         /// 모르겠음(^^;)

	private:
		ZQuadTree* m_pChild[4];  /// QuadTree의 4개의 자식노드

		/// Triangle Crack(Popping)을 막기위해서 사용한다.
		ZQuadTree*      m_pParent;
		ZQuadTree*      m_pNeighbor[4];

		int m_nCenter;         /// QuadTree에 보관할 첫번째 값
		int m_nCorner[4];      /// QuadTree에 보관할 두번째 값

		BOOL  m_bCulled;        ///프러스텀에서 컬링된 노드인가?
		float m_fRadius;		/// 노드를 감싸는 경계구(bounding sphere)의 반지름

	private:
		/// 자식 노드를 추가한다.
		ZQuadTree* _AddChild(int nCornerTL, int nCornerTR, int nCornerBL, int nCornerBR);

		/// 4개의 코너값을 셋팅한다.
		BOOL _SetCorners(int nCornerTL, int nCornerTR, int nCornerBL, int nCornerBR);

		/// Quadtree를 4개의 하위 트리로 부분분할(subdivide)한다.
		BOOL _SubDivide();

		/// 현재 노드가 출력이 가능한 노드인가?
		BOOL _IsVisible() { return (m_nCorner[CORNER_TR] - m_nCorner[CORNER_TL] <= 1); }

		/// 출력할 폴리곤의 인덱스를 생성한다.
		int _GenTriIndex(int nTris, LPVOID pIndex, VERTEX* pHeightMap, ZFrustum* pFrustum, float fLODRatio);

		/// 두점 사이의 거리를 구한다.
		float _GetDistance(D3DXVECTOR3* pv1, D3DXVECTOR3* pv2);

		/// 카메라와 현재 노드와의 거리값을 기준으로 LOD값을 구한다.
		int _GetLODLevel(VERTEX* pHeightMap, D3DXVECTOR3* pCamera, float fLODRatio);

		/// 이웃노드를 만든다.(삼각형 찢어짐 방지용)
		void _BuildNeighborNode(ZQuadTree* pRoot, VERTEX* pHeightMap, int cx);

		/// 4개 방향(상단,하단,좌측,우측)의 이웃노드 인덱스를 구한다.
		int  _GetNodeIndex(int ed, int cx, int& _0, int& _1, int& _2, int& _3);

		// 쿼드트리를 검색해서 4개 코너값과 일치하는 노드를 찾는다.
		ZQuadTree* _FindNode(VERTEX* pHeightMap, int _0, int _1, int _2, int _3);

		/// 현재노드가 프러스텀에 포함되는가?
		int _IsInFrustum(VERTEX* pHeightMap, ZFrustum* pFrustum);

		/// _IsInFrustum()함수의 결과에 따라 프러스텀 컬링 수행
		void _FrustumCull(VERTEX* pHeightMap, ZFrustum* pFrustum);

		//// 쿼드트리를 만든다.(Build()함수에서 불린다)
		BOOL _BuildQuadTree(VERTEX* pHeightMap);

		/// 메모리에서 쿼드트리를 삭제한다.
		void _Destroy();

	public:    
		/// 최초 루트노드 생성자
		ZQuadTree(int cx, int cy);

		/// 하위 자식노드 생성자
		ZQuadTree(ZQuadTree* pParent);

		/// 소멸자
		~ZQuadTree();

		/// QuadTree를 구축한다.
		BOOL Build(VERTEX* pHeightMap);

		/// 현재 노드가 LOD등급으로 볼때  출력이 가능한 노드인가?
		BOOL IsVisible(VERTEX* pHeightMap, D3DXVECTOR3* pCamera, float fLODRatio);
		
		/// 코너 인덱스 값을 얻어온다.
		void GetCorner(int& _0, int& _1, int& _2, int& _3);

		/// 삼각형의 인덱스를 만들고, 출력할 삼각형의 개수를 반환한다.
		int GenerateIndex(LPVOID pIndex, VERTEX* pHeightMap, ZFrustum* pFrustum, float fLODRatio);
	};
}