#pragma once
#include "Common.h"
#include "Vertex.h"
#include "ZQuadTree.h"
#include "ZFrustum.h"

namespace BONE_GRAPHICS
{
	struct TerrainInfo
	{
		// ���̸� �ڷḦ ���� Raw������ �̸�.
		char* HeigtMapFileName;

		// ������ ���� �ؽ�ó �̹����� ���� �̸���.
		LPDIRECT3DTEXTURE9 AlphaMap[4];
		LPDIRECT3DTEXTURE9 LayerMapFile[5];

		// ũ�� ���
		float HeightScale;
		int TileScale;

		// ���̸��� ũ��
		LPDIRECT3DTEXTURE9 HeightMap;
		DWORD WidthSize;
		DWORD HeightSize;

		// Mesh
		VERTEX* Vertex;
		VERTEX_INDEX* Vertex_Index;
		LPDIRECT3DVERTEXBUFFER9 VertexBuffer;
		int VertexCount;
		LPDIRECT3DINDEXBUFFER9 IndexBuffer;
		int IndexCount;
		int Triangles;
	};

	struct TerrainBrushInfo {
		int DotCount;	// �� ����
		float Radian;	// ���� ��
		float Radiuse[2];	// �귯�� ������
		float Strong;// ����
		float BrushMode;
		float Height;
		int CurrentTextureIndex;
		bool Invert;
		enum { TEXTURE, HEIGHT_1, HEIGHT_2, NATURE };
	};

	class etuTerrain
	{
	private:
		TerrainInfo Terrain_Info;
		TerrainBrushInfo Brush_Info;
		bool Is_Init;
		LPD3DXEFFECT Shader_Terrain_Shadow;
		LPD3DXEFFECT Shader_CreateShadowMap;
		ZQuadTree* m_pQuadTree;
		ZFrustum m_pFrustum;
		D3DXMATRIX m_matWorld;

		void InitVertexBuffer();
		void InitIndexBuffer();

	public:
		void Initialize(float _fHeightScale, float _fTileScale);
		void InitializeFromData(string _Address);
		void SaveData(string _Address);

		void LoadBrushTexture(int _Index, char* _String);
		void SetBrush(float _fTopRadiuse, float _fBottomRadiuse, float _fHeight, float _fStrong);
		void SetBrsuh(bool Invert);
		void ChanageBrushMode(int _Mode);
		void UpdateBrush(float _fTimeDelta);
		void RenderBrush();
		void SelectTextureIndex(int _Index);

		void Render_CreateShadow();
		int GetBrushMode();
		D3DXVECTOR3 FindVertexUseRay(Ray* _PickingRay);
		TerrainInfo GetInfo();
		TerrainBrushInfo GetBrushInfo();

		D3DXMATRIX GetWorldMatrix();
		void Update(D3DXMATRIXA16* _matVeiwProj, D3DXVECTOR3 _vec3CameraPos);

		bool DrawAlphamaps(int nIndex);
		void MiniAlphaTextureRender();
		BOOL CreateAlphaMaps();
		void RenderFrustum();

		void SaveAlphaMap();

		void Render(D3DXVECTOR4 _vec3Light, LPDIRECT3DBASETEXTURE9 _ShdowMap, D3DXMATRIX _matViewProjection, D3DXMATRIX _matLightView, D3DXMATRIX _matLightProjection);

		void Release();
	};
}