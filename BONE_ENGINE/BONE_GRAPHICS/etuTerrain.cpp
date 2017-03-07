#include "Common.h"
#include "etuTerrain.h"
#include "InputManager.h"
#include "RenderManager.h"
#include "LogManager.h"
#include "Vertex.h"
#pragma warning(disable:4996)

namespace BONE_GRAPHICS
{
	void etuTerrain::Initialize(float _fHeightScale, float _fTileScale)
	{
		CreateAlphaMaps();

		/// �⺻ Brush ����
		Brush_Info.DotCount = 50;
		Brush_Info.Radian = D3DX_PI * 2.0f / Brush_Info.DotCount;
		Brush_Info.Radiuse[0] = 5.0f;
		Brush_Info.Radiuse[1] = 3.0f;
		Brush_Info.Strong = 1.0f;
		Brush_Info.Height = 5.0f;
		Brush_Info.BrushMode = TerrainBrushInfo::HEIGHT_1;
		Brush_Info.Invert = false;

		/// etuTerrain �⺻ ����
		Terrain_Info.HeightMap = nullptr;
		Terrain_Info.VertexBuffer = nullptr;
		Terrain_Info.IndexBuffer = nullptr;
		Terrain_Info.HeightScale = _fHeightScale;
		Terrain_Info.TileScale = _fTileScale;

		Terrain_Info.WidthSize = 129;
		Terrain_Info.HeightSize = 129;

		D3DXMatrixIdentity(&m_matWorld);
		D3DXMatrixScaling(&m_matWorld, Terrain_Info.HeightScale, 1, Terrain_Info.HeightScale);

		m_pQuadTree = nullptr;
		m_pQuadTree = new ZQuadTree(Terrain_Info.WidthSize, Terrain_Info.HeightSize);

		LoadBrushTexture(0, "Resource\\Environment\\Terrain\\TileMap.jpg");
		LoadBrushTexture(1, "Resource\\Environment\\Terrain\\TileMap1.jpg");
		LoadBrushTexture(2, "Resource\\Environment\\Terrain\\TileMap2.jpg");
		LoadBrushTexture(3, "Resource\\Environment\\Terrain\\TileMap3.jpg");
		LoadBrushTexture(4, "Resource\\Environment\\Terrain\\TileMap4.jpg");

		//Shader_Terrain_Shadow = RenderMgr->LoadShader("Resource\\Shader\\Terrain_ApplyShadow.fx");

		InitVertexBuffer();
		InitIndexBuffer();

		m_pQuadTree->Build(Terrain_Info.Vertex);

		Is_Init = true;
	}

	void etuTerrain::InitializeFromData(string _Address)
	{
		CreateAlphaMaps();

		/// �⺻ Brush ����
		Brush_Info.DotCount = 50;
		Brush_Info.Radian = D3DX_PI * 2.0f / Brush_Info.DotCount;
		Brush_Info.Radiuse[0] = 5.0f;
		Brush_Info.Radiuse[1] = 3.0f;
		Brush_Info.Strong = 1.0f;
		Brush_Info.Height = 5.0f;
		Brush_Info.BrushMode = TerrainBrushInfo::HEIGHT_1;
		Brush_Info.Invert = false;

		/// etuTerrain �⺻ ����
		Terrain_Info.HeightMap = nullptr;
		Terrain_Info.VertexBuffer = nullptr;
		Terrain_Info.IndexBuffer = nullptr;

		Terrain_Info.WidthSize = 129;
		Terrain_Info.HeightSize = 129;

		m_pQuadTree = nullptr;
		m_pQuadTree = new ZQuadTree(Terrain_Info.WidthSize, Terrain_Info.HeightSize);

		LoadBrushTexture(0, "Resource\\Environment\\Terrain\\TileMap.jpg");
		LoadBrushTexture(1, "Resource\\Environment\\Terrain\\TileMap1.jpg");
		LoadBrushTexture(2, "Resource\\Environment\\Terrain\\TileMap2.jpg");
		LoadBrushTexture(3, "Resource\\Environment\\Terrain\\TileMap3.jpg");
		LoadBrushTexture(4, "Resource\\Environment\\Terrain\\TileMap4.jpg");

		//Shader_Terrain_Shadow = RenderMgr->LoadShader("Resource\\Shader\\Terrain_ApplyShadow.fx");

		Is_Init = true;

		string XmlAddr = _Address + "\\Settings.xml";
		
		TiXmlDocument doc;
		doc.LoadFile(XmlAddr.c_str());

		// ��Ʈ ��� ����
		TiXmlElement* pRoot = doc.FirstChildElement("Terrain");
		
		if (!pRoot)
		{
			LogMgr->ShowMessage(LOG_ERROR, "Failed Loading - Settings.xml");
			return;
		}

		// Terrain Information
		TiXmlElement* pElem = pRoot->FirstChildElement("Terrain_Information");

		if (!pElem)
		{
			LogMgr->ShowMessage(LOG_ERROR, "Failed File - Settings.xml");
			return;
		}

		double HeightMapScale = 0, TileMapScale = 0;

		TiXmlElement* pTerrainInfoElem = pElem->FirstChildElement("Scale");
		
		if (!pTerrainInfoElem)
		{
			LogMgr->ShowMessage(LOG_ERROR, "Failed File - Settings.xml");
			return;
		}

		pTerrainInfoElem->Attribute("HeightMap", &HeightMapScale);
		Terrain_Info.HeightScale = (float)HeightMapScale;

		pTerrainInfoElem->Attribute("TileMap", &TileMapScale);
		Terrain_Info.TileScale = (float)TileMapScale;

		
		for (int i = 0; i < 4; i++)
		{
			string Address;

			pTerrainInfoElem = pTerrainInfoElem->NextSiblingElement();
			Address = pTerrainInfoElem->Attribute("Address");

			if (FAILED(D3DXCreateTextureFromFile(RenderMgr->GetDevice(), Address.c_str(), &Terrain_Info.AlphaMap[i])))
			{
				LogMgr->ShowMessage(LOG_ERROR, "Terrain %d-AlphaMap is not load.", i);
				return;
			}
		}

		// Height Data
		TiXmlElement* pHeightElem = pElem->NextSiblingElement("Height_Data")->FirstChildElement("z");
		int length = 0;

		if (Terrain_Info.Vertex == nullptr)
		{
			Terrain_Info.Vertex = new VERTEX[Terrain_Info.HeightSize * Terrain_Info.WidthSize];
		}

		for (int z = 0; z < 129; z++)
		{
			TiXmlElement* pDataElem = pHeightElem->FirstChildElement("x");

			for (int x = 0; x < 129; x++)
			{
				Terrain_Info.Vertex[length++].p.y = atof(pDataElem->GetText());
				pDataElem = pDataElem->NextSiblingElement();
			}

			pHeightElem = pHeightElem->NextSiblingElement();
		}

		D3DXMatrixIdentity(&m_matWorld);
		D3DXMatrixScaling(&m_matWorld, Terrain_Info.HeightScale, 1, Terrain_Info.HeightScale);

		InitVertexBuffer();
		InitIndexBuffer();

		m_pQuadTree->Build(Terrain_Info.Vertex);
	}

	void etuTerrain::SaveData(string _Address)
	{
		SaveAlphaMap();

		// XML ���� ����
		TiXmlDocument doc;
		TiXmlDeclaration* pDecl = new TiXmlDeclaration("1.0", "euc-kr", "");
		doc.LinkEndChild(pDecl);

		// Root ��� �߰�
		TiXmlElement* pRoot = new TiXmlElement("Terrain");
		doc.LinkEndChild(pRoot);

		// Comment(�ּ�) ���� �߰�.
		TiXmlComment* pComment = new TiXmlComment();
		pComment->SetValue("Settings for Terrain");
		pRoot->LinkEndChild(pComment);


		// Terrain_Information
		TiXmlElement* pElem = new TiXmlElement("Terrain_Information");
		pRoot->LinkEndChild(pElem);

		TiXmlElement* pTerrainInfoElem = new TiXmlElement("Scale");
		pElem->LinkEndChild(pTerrainInfoElem);
		pTerrainInfoElem->SetDoubleAttribute("HeightMap", Terrain_Info.HeightScale);
		pTerrainInfoElem->SetDoubleAttribute("TileMap", Terrain_Info.TileScale);

		for (int i = 0; i < 4; i++)
		{
			TiXmlElement* pTextureElem = new TiXmlElement("AlphaMap");
			pElem->LinkEndChild(pTextureElem);
			pTextureElem->SetDoubleAttribute("ID", i);

			string addr = _Address + "\\AlphaMap";

			char Number[3];
			itoa(i, Number, 10);

			addr += Number;
			addr += ".jpg";
			
			pTextureElem->SetAttribute("Address", addr.c_str());
		}


		// Height_Data
		pElem = new TiXmlElement("Height_Data");
		pRoot->LinkEndChild(pElem);
		int length = 0;

		for (int z = 0; z < 129; z++)
		{
			TiXmlElement* zPosElem = new TiXmlElement("z");
			pElem->LinkEndChild(zPosElem);

			for (int x = 0; x < 129; x++)
			{
				char Height[20] = "";
				_gcvt(Terrain_Info.Vertex[length++].p.y, 10, Height);
				//itoa(Terrain_Info.Vertex->p.y, Height, 10);
				Height[strlen(Height)] = '\0';

				TiXmlElement* xPosElem = new TiXmlElement("x");
				xPosElem->LinkEndChild(new TiXmlText(Height));
				zPosElem->LinkEndChild(xPosElem);
			}
		}

		p_sAddress += "\\Settings.xml";

		// XML ���� ���·� ����.
		doc.SaveFile(p_sAddress.c_str());
	}

	void etuTerrain::SaveAlphaMap()
	{
		D3DXSaveTextureToFile("MapData\\AlphaMap0.jpg", D3DXIFF_JPG, Terrain_Info.AlphaMap[0], nullptr);
		D3DXSaveTextureToFile("MapData\\AlphaMap1.jpg", D3DXIFF_JPG, Terrain_Info.AlphaMap[1], nullptr);
		D3DXSaveTextureToFile("MapData\\AlphaMap2.jpg", D3DXIFF_JPG, Terrain_Info.AlphaMap[2], nullptr);
		D3DXSaveTextureToFile("MapData\\AlphaMap3.jpg", D3DXIFF_JPG, Terrain_Info.AlphaMap[3], nullptr);
	}

	void etuTerrain::Update(D3DXMATRIXA16* _matVeiwProj, D3DXVECTOR3 _vec3CameraPos)
	{
		m_pFrustum.Make(p_matVeiwProj, _vec3CameraPos);
	}

	void etuTerrain::LoadBrushTexture(int _Index, char* _String)
	{
		if (FAILED(D3DXCreateTextureFromFile(RenderMgr->GetDevice(), _String, &Terrain_Info.LayerMapFile[p_iIndex])))
		{
			LogMgr->ShowMessage(LOG_ERROR, "Trrain %f-Texture is not load.", _Index);
		}
	}

	void etuTerrain::InitVertexBuffer()
	{
		if (Terrain_Info.Vertex == nullptr)
		{
			Terrain_Info.Vertex = new VERTEX[Terrain_Info.HeightSize * Terrain_Info.WidthSize];
		}

		if (!Terrain_Info.VertexBuffer && FAILED(RenderMgr->GetDevice()->CreateVertexBuffer(Terrain_Info.HeightSize * Terrain_Info.WidthSize * sizeof(VERTEX),
			D3DUSAGE_WRITEONLY, VERTEX::FVF, D3DPOOL_DEFAULT, &Terrain_Info.VertexBuffer, nullptr)))
		{
			return;
		}

		D3DLOCKED_RECT d3drc;

		VOID* pVertices;

		// ���� ���� ��
		if (FAILED(Terrain_Info.VertexBuffer->Lock(0, Terrain_Info.HeightSize * Terrain_Info.WidthSize * sizeof(VERTEX), (void**)&pVertices, 0)))
		{
			return;
		}

		VERTEX v;
		VERTEX* pV = (VERTEX*)pVertices;
		int length = 0;

		for (int z = 0; z < Terrain_Info.HeightSize; z++)
		{
			for (int x = 0; x < Terrain_Info.WidthSize; x++)
			{
				v.p.x = (float)x - Terrain_Info.WidthSize / 2.0f;
				v.p.z = -((float)z - Terrain_Info.HeightSize / 2.0f);

				if (Is_Init)
					v.p.y = Terrain_Info.Vertex[length].p.y;
				else
					v.p.y = 0;

				D3DXVec3TransformCoord(&v.p, &v.p, &m_matWorld);

				v.n = v.p;
				D3DXVec3Normalize(&v.n, &v.n);
				
				v.uv.x = (float)x / (Terrain_Info.WidthSize - 1);
				v.uv.y = (float)z / (Terrain_Info.HeightSize - 1);
				
				*(pV++) = v;

				Terrain_Info.Vertex[length++] = v;
			}
		}

		Terrain_Info.VertexBuffer->Unlock();
	}

	void etuTerrain::RenderFrustum()
	{
		m_pFrustum.Draw(RenderMgr->GetDevice());
	}

	void etuTerrain::InitIndexBuffer()
	{
		if (!Terrain_Info.IndexBuffer && FAILED(RenderMgr->GetDevice()->CreateIndexBuffer((Terrain_Info.HeightSize - 1) * (Terrain_Info.WidthSize - 1) * 2 * sizeof(VERTEX_INDEX),
			0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &Terrain_Info.IndexBuffer, nullptr)))
		{
			return;
		}

		VERTEX_INDEX i;
		VERTEX_INDEX* pI;

		// ���� ���� ��
		if (FAILED(Terrain_Info.IndexBuffer->Lock(0, (Terrain_Info.HeightSize - 1) * (Terrain_Info.WidthSize - 1) * 2 * sizeof(VERTEX_INDEX),
			(void**)&pI, 0)))
		{
			return;
		}

		for (int z = 0; z < Terrain_Info.HeightSize - 1; z++)
		{
			for (int x = 0; x < Terrain_Info.WidthSize - 1; x++)
			{
				// ���� �ε����� CW �������� �����ϰ� �ִ�.  
				// ���� CW �������� �ø� ��带 �����ϸ� �ƹ��͵� �׷����� ���� ���̴�.  
				// g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW ) �� ���   
				// �׷����� ���� ����.  

				// �»� ��ġ�� �ﰢ��  
				i._0 = (z*Terrain_Info.WidthSize + x);
				i._1 = (z*Terrain_Info.WidthSize + x + 1);
				i._2 = ((z + 1)*Terrain_Info.WidthSize + x);
				*pI++ = i;

				// ���� ��ġ�� �ﰢ��  
				i._0 = ((z + 1)*Terrain_Info.WidthSize + x);
				i._1 = (z*Terrain_Info.WidthSize + x + 1);
				i._2 = ((z + 1)*Terrain_Info.WidthSize + x + 1);
				*pI++ = i;

			}
		}
		Terrain_Info.IndexBuffer->Unlock();

		//m_pQuadTree->Build(Terrain_Info.Vertex);
	}

	D3DXVECTOR3 etuTerrain::FindVertexUseRay(RAY* _PickingRay)
	{
		static D3DXVECTOR3 LastPick;
		float u, v;          // ��ŷ���� u v�� �������� (���Ŀ� ���� �ʿ���)
		float dist;          // �ﰢ���� ��ŷ������ �Ÿ�

		RAY ray;

		if (p_PickingRay == nullptr)
			ray = RenderMgr->GetPickingRayToView(false);
		else
			ray = *p_PickingRay;

		D3DXMATRIX matWorld;
		D3DXMatrixIdentity(&matWorld);
		RenderMgr->GetDevice()->GetTransform(D3DTS_WORLD, &matWorld);

		for (int z = 0; z < Terrain_Info.HeightSize - 1; z++)
		{
			for (int x = 0; x < Terrain_Info.WidthSize - 1; x++)
			{
				// �»� ��ġ�� �ﰢ��  
				D3DXVECTOR3 Temp[3];

				Temp[0] = Terrain_Info.Vertex[(z * Terrain_Info.WidthSize + x)].p;
				Temp[1] = Terrain_Info.Vertex[(z * Terrain_Info.WidthSize + x + 1)].p;
				Temp[2] = Terrain_Info.Vertex[((z + 1) * Terrain_Info.WidthSize + x)].p;

				if (RenderMgr->CheckRayInTriangle(&ray, matWorld, Temp[0], Temp[1], Temp[2], &u, &v, &dist))
				{
					// ��ŷ ��ġ ���ϴ� ��
					D3DXVECTOR3 Result = Temp[0] + v *(Temp[1] - Temp[0]) + u * (Temp[2] - Temp[0]);
					LastPick = Result;
					return Result;
				}

				// ���� ��ġ�� �ﰢ��  
				Temp[0] = Terrain_Info.Vertex[((z + 1) * Terrain_Info.WidthSize + x)].p;
				Temp[1] = Terrain_Info.Vertex[(z * Terrain_Info.WidthSize + x + 1)].p;
				Temp[2] = Terrain_Info.Vertex[((z + 1) * Terrain_Info.WidthSize + x + 1)].p;

				if (RenderMgr->CheckRayInTriangle(&ray, matWorld, Temp[0], Temp[1], Temp[2], &u, &v, &dist))
				{
					D3DXVECTOR3 Result = Temp[0] + v *(Temp[1] - Temp[0]) + u * (Temp[2] - Temp[0]);
					LastPick = Result;
					return Result;
				}
			}
		}
		return LastPick;
	}

	void etuTerrain::Render(D3DXVECTOR4 _vec3Light, LPDIRECT3DBASETEXTURE9 _ShdowMap, D3DXMATRIX _matViewProjection, D3DXMATRIX _matLightView, D3DXMATRIX _matLightProjection)
	{
		// �׸��� ������ ���̴� ������������ ����
		Shader_Terrain_Shadow->SetMatrix("gViewProjectionMatrix", &p_matViewProjection);
		Shader_Terrain_Shadow->SetMatrix("gLightViewMatrix", &p_matLightView);
		Shader_Terrain_Shadow->SetMatrix("gLightProjectionMatrix", &p_matLightProjection);

		D3DXVECTOR4 AmbientLight(1, 1, 1, 1);
		Shader_Terrain_Shadow->SetVector("gObjectColor", &AmbientLight);

		Shader_Terrain_Shadow->SetVector("gWorldLightPosition", &p_vec3Light);
		Shader_Terrain_Shadow->SetTexture("ShadowMap_Tex", _ShdowMap);
		// ���̴��� �����Ѵ�.

		D3DXMATRIXA16 matWorld;
		D3DXMatrixIdentity(&matWorld);
		RenderMgr->GetDevice()->SetTransform(D3DTS_WORLD, &matWorld);

		Shader_Terrain_Shadow->SetMatrix("gWorldMatrix", &matWorld);
		Shader_Terrain_Shadow->SetFloat("gTileScale", Terrain_Info.TileScale);
		Shader_Terrain_Shadow->SetTexture("TileLayer0_Tex", Terrain_Info.LayerMapFile[0]);
		Shader_Terrain_Shadow->SetTexture("TileLayer1_Tex", Terrain_Info.LayerMapFile[1]);
		Shader_Terrain_Shadow->SetTexture("TileLayer2_Tex", Terrain_Info.LayerMapFile[2]);
		Shader_Terrain_Shadow->SetTexture("TileLayer3_Tex", Terrain_Info.LayerMapFile[3]);
		Shader_Terrain_Shadow->SetTexture("TileLayer4_Tex", Terrain_Info.LayerMapFile[4]);
		Shader_Terrain_Shadow->SetTexture("AlphaLayer1_Tex", Terrain_Info.AlphaMap[0]);
		Shader_Terrain_Shadow->SetTexture("AlphaLayer2_Tex", Terrain_Info.AlphaMap[1]);
		Shader_Terrain_Shadow->SetTexture("AlphaLayer3_Tex", Terrain_Info.AlphaMap[2]);
		Shader_Terrain_Shadow->SetTexture("AlphaLayer4_Tex", Terrain_Info.AlphaMap[3]);
		Shader_Terrain_Shadow->CommitChanges();

		LPWORD pI;

		Terrain_Info.IndexBuffer->Lock(0, (Terrain_Info.WidthSize - 1) * (Terrain_Info.HeightSize - 1) * 2 * sizeof(VERTEX_INDEX), (void**)&pI, 0);
		Terrain_Info.Triangles = m_pQuadTree->GenerateIndex(pI, Terrain_Info.Vertex, &m_pFrustum, 0.005f);
		Terrain_Info.IndexBuffer->Unlock();

		// �׸��� ����� ���̴��� ����
		{
			UINT numPasses = 0;

			Shader_Terrain_Shadow->Begin(&numPasses, nullptr);
			{
				for (UINT i = 0; i < numPasses; ++i)
				{
					Shader_Terrain_Shadow->BeginPass(i);
					{
						RenderMgr->GetDevice()->SetStreamSource(0, Terrain_Info.VertexBuffer, 0, sizeof(VERTEX));
						RenderMgr->GetDevice()->SetIndices(Terrain_Info.IndexBuffer);
						RenderMgr->GetDevice()->SetFVF(VERTEX::FVF);
					  /*RenderMgr->GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0,
							Terrain_Info.HeightSize * Terrain_Info.WidthSize, 0, (Terrain_Info.HeightSize - 1) * (Terrain_Info.WidthSize - 1) * 2);*/
						RenderMgr->GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0,
							Terrain_Info.HeightSize * Terrain_Info.WidthSize, 0, Terrain_Info.Triangles);
					}
					Shader_Terrain_Shadow->EndPass();
				}
			}
			Shader_Terrain_Shadow->End();
		}
	}

	BOOL etuTerrain::CreateAlphaMaps()
	{
		for (int i = 0; i<4; ++i)
		{
			if (FAILED(RenderMgr->GetDevice()->CreateTexture(1024, 1024, 1, 0, D3DFMT_A8R8G8B8,
				D3DPOOL_MANAGED, &Terrain_Info.AlphaMap[i], nullptr)))
				return MessageBox(nullptr, "alphaTexture make error", "Error", MB_OK);

			D3DLOCKED_RECT		AlphaMap_Locked;

			memset(&AlphaMap_Locked, 0, sizeof(D3DLOCKED_RECT));

			if (FAILED(Terrain_Info.AlphaMap[i]->LockRect(0, &AlphaMap_Locked, nullptr, 0)))
				return MessageBox(nullptr, "alphaTexture Lock error", "Error", MB_OK);

			LPBYTE pDataDST = (LPBYTE)AlphaMap_Locked.pBits;

			for (int j = 0; j<Terrain_Info.HeightSize; ++j)
			{
				LPDWORD pDWordDST = (LPDWORD)(pDataDST + j * AlphaMap_Locked.Pitch);

				for (int i = 0; i<Terrain_Info.WidthSize; ++i)
				{
					*(pDWordDST + i) = 0x00000000;
				}
			}

			if (FAILED(Terrain_Info.AlphaMap[i]->UnlockRect(0)))
				return MessageBox(nullptr, "alphaTexture UnLock error", "Error", MB_OK);
		}

		return TRUE;
	}

	bool etuTerrain::DrawAlphamaps(int nIndex)
	{
		const int MAPWIDTH = Terrain_Info.WidthSize + 1;
		const int MAPHEIGHT = Terrain_Info.HeightSize + 1;
		const int TEXALPHASIZE = 1024;

		// ���ĸ��� �� �ȼ��� ���� ���� ������ ũ�⸦ ���Ѵ�.
		// 1.0f <-���� �簢��1�����׷����Ƿ�
		//float		PixSize		= 1.0f/(float)TEXALPHASIZE;
		float		PixSize = (float)MAPWIDTH / (float)TEXALPHASIZE;

		//�����귯�� �� ������������ ���� �귯���ҿ��� ������ �ֱ�빮�� �������� ����ؾ��Ѵ�.
		float 	    fHalfBrushSize = Brush_Info.Radiuse[0];
		float		fHalfSmoothSize = Brush_Info.Strong;

		// �������� ���� �ִ� ũ�⿡ ������ ũ�⸦ ��������
		// ���ȼ��� ������ �ϴ��� ũ�⸦ ���Ѵ�.
		float		EditSize = fHalfBrushSize / PixSize;

		D3DXMATRIX matInverse;
		D3DXMatrixInverse(&matInverse, nullptr, &m_matWorld);

		D3DXVECTOR3 RayPosition = FindVertexUseRay(nullptr);
		D3DXVec3TransformCoord(&RayPosition, &RayPosition, &matInverse);

		RayPosition.x += Terrain_Info.WidthSize / 2.0f;
		RayPosition.z += Terrain_Info.HeightSize / 2.0f;

		float	PosU = RayPosition.x / (float)(MAPWIDTH);
		float	PosV = 1 - RayPosition.z / (float)(MAPHEIGHT);
		//float	PosV = RayPosition.z / (float)(MAPHEIGHT);

		int m_nTexPosX = int(PosU  * (float)TEXALPHASIZE);
		int m_nTexPosY = int(PosV  * (float)TEXALPHASIZE);

		int StartPosX = int(((m_nTexPosX - EditSize) < 0) ? 0 : m_nTexPosX - EditSize);
		int StartPosY = int(((m_nTexPosY - EditSize) < 0) ? 0 : m_nTexPosY - EditSize);
		int EndPosX = int(((m_nTexPosX + EditSize) >= TEXALPHASIZE) ? TEXALPHASIZE - 1 : m_nTexPosX + EditSize);
		int EndPosY = int(((m_nTexPosY + EditSize) >= TEXALPHASIZE) ? TEXALPHASIZE - 1 : m_nTexPosY + EditSize);

		int dwChangeColor = 0x00;
		float Smooth = 0.0f;
		int dwA = 0x00;

		D3DLOCKED_RECT		AlphaMap_Locked;

		memset(&AlphaMap_Locked, 0, sizeof(D3DLOCKED_RECT));

		if (FAILED(Terrain_Info.AlphaMap[nIndex]->LockRect(0, &AlphaMap_Locked, nullptr, 0)))
		{
			MessageBox(nullptr, "alphaTexture Lock error", "Error", MB_OK);
			return FALSE;
		}

		LPBYTE pDataDST = (LPBYTE)AlphaMap_Locked.pBits;

		for (int j = StartPosY; j <= EndPosY; ++j)
		{
			LPDWORD pDWordDST = (LPDWORD)(pDataDST + j * AlphaMap_Locked.Pitch);

			for (int i = StartPosX; i <= EndPosX; ++i)
			{
				D3DXVECTOR3 Pix = D3DXVECTOR3(i * PixSize, 0.0f, j * PixSize) -
					D3DXVECTOR3(m_nTexPosX * PixSize, 0.0f, m_nTexPosY * PixSize);

				float Len = D3DXVec3Length(&Pix);

				if (Len <= fHalfSmoothSize)
				{
					dwChangeColor = 0xFFFFFFFF;

					if (Brush_Info.Invert)
						dwChangeColor = 0x00000000;
				}
				else if (Len <= fHalfBrushSize)
				{
					// �ִ����� ���Ե� �� �ִ�����������
					// ���������� ���İ��� ���Ѵ�.
					Len -= fHalfSmoothSize;
					Smooth = fHalfBrushSize - fHalfSmoothSize;

					if (!Brush_Info.Invert)
					{
						dwChangeColor = int((Smooth - Len) / Smooth * 0xFF000000);
						dwA = (dwChangeColor & 0xFF000000) >> 24;

						dwChangeColor = (dwA << 24) | (dwA << 16) | (dwA << 8) | dwA;
					}
					else
					{
						dwChangeColor = int((Smooth - Len) / Smooth * 0x00000000);
					}
				}
				else
				{
					continue;
				}

				if (Brush_Info.Invert)
					*(pDWordDST + i) = (*(pDWordDST + i) > dwChangeColor) ? dwChangeColor : *(pDWordDST + i);
				else
					*(pDWordDST + i) = (*(pDWordDST + i) < dwChangeColor) ? dwChangeColor : *(pDWordDST + i);
			}
		}

		if (FAILED(Terrain_Info.AlphaMap[nIndex]->UnlockRect(0)))
		{
			MessageBox(nullptr, "alphaTexture UnLock error", "Error", MB_OK);
			return FALSE;
		}

		return TRUE;
	}

	D3DXMATRIX etuTerrain::GetWorldMatrix()
	{
		return m_matWorld;
	}

	void etuTerrain::Render_CreateShadow()
	{
		/*LPWORD pI;
		Terrain_Info.IndexBuffer->Lock(0, (Terrain_Info.WidthSize - 1) * (Terrain_Info.HeightSize - 1) * 2 * sizeof(VERTEX_INDEX), (void**)&pI, 0);
		Terrain_Info.Triangles = m_pQuadTree->GenerateIndex(pI, Terrain_Info.Vertex, &m_pFrustum, 0.02f);
		Terrain_Info.IndexBuffer->Unlock();

		D3DXMATRIXA16 matWorld;
		D3DXMatrixIdentity(&matWorld);
		RenderMgr->GetDevice()->SetTransform(D3DTS_WORLD, &matWorld);

		RenderMgr->GetDevice()->SetStreamSource(0, Terrain_Info.VertexBuffer, 0, sizeof(VERTEX));
		RenderMgr->GetDevice()->SetIndices(Terrain_Info.IndexBuffer);
		RenderMgr->GetDevice()->SetFVF(VERTEX::FVF);
		RenderMgr->GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0,
			Terrain_Info.HeightSize * Terrain_Info.WidthSize, 0, Terrain_Info.Triangles);*/

			// �׸��� ������ ���̴� ������������ ����
		//Shader_Terrain_Shadow->SetMatrix("gViewProjectionMatrix", &p_matViewProjection);
		//Shader_Terrain_Shadow->SetMatrix("gLightViewMatrix", &p_matLightView);
		//Shader_Terrain_Shadow->SetMatrix("gLightProjectionMatrix", &p_matLightProjection);

		D3DXVECTOR4 AmbientLight(1, 1, 1, 1);
		Shader_Terrain_Shadow->SetVector("gObjectColor", &AmbientLight);

		//Shader_Terrain_Shadow->SetVector("gWorldLightPosition", &p_vec3Light);
		//Shader_Terrain_Shadow->SetTexture("ShadowMap_Tex", _ShdowMap);
		// ���̴��� �����Ѵ�.

		D3DXMATRIXA16 matWorld;
		D3DXMatrixIdentity(&matWorld);
		RenderMgr->GetDevice()->SetTransform(D3DTS_WORLD, &matWorld);

		Shader_Terrain_Shadow->SetMatrix("gWorldMatrix", &matWorld);
		Shader_Terrain_Shadow->SetFloat("gTileScale", Terrain_Info.TileScale);
		Shader_Terrain_Shadow->SetTexture("TileLayer0_Tex", Terrain_Info.LayerMapFile[0]);
		Shader_Terrain_Shadow->SetTexture("TileLayer1_Tex", Terrain_Info.LayerMapFile[1]);
		Shader_Terrain_Shadow->SetTexture("TileLayer2_Tex", Terrain_Info.LayerMapFile[2]);
		Shader_Terrain_Shadow->SetTexture("TileLayer3_Tex", Terrain_Info.LayerMapFile[3]);
		Shader_Terrain_Shadow->SetTexture("TileLayer4_Tex", Terrain_Info.LayerMapFile[4]);
		Shader_Terrain_Shadow->SetTexture("AlphaLayer1_Tex", Terrain_Info.AlphaMap[0]);
		Shader_Terrain_Shadow->SetTexture("AlphaLayer2_Tex", Terrain_Info.AlphaMap[1]);
		Shader_Terrain_Shadow->SetTexture("AlphaLayer3_Tex", Terrain_Info.AlphaMap[2]);
		Shader_Terrain_Shadow->SetTexture("AlphaLayer4_Tex", Terrain_Info.AlphaMap[3]);
		Shader_Terrain_Shadow->CommitChanges();

		LPWORD pI;

		Terrain_Info.IndexBuffer->Lock(0, (Terrain_Info.WidthSize - 1) * (Terrain_Info.HeightSize - 1) * 2 * sizeof(VERTEX_INDEX), (void**)&pI, 0);
		Terrain_Info.Triangles = m_pQuadTree->GenerateIndex(pI, Terrain_Info.Vertex, &m_pFrustum, 0.005f);
		Terrain_Info.IndexBuffer->Unlock();

		// �׸��� ����� ���̴��� ����
		{
			UINT numPasses = 0;

			Shader_Terrain_Shadow->Begin(&numPasses, nullptr);
			{
				for (UINT i = 0; i < numPasses; ++i)
				{
					Shader_Terrain_Shadow->BeginPass(i);
					{
						RenderMgr->GetDevice()->SetStreamSource(0, Terrain_Info.VertexBuffer, 0, sizeof(VERTEX));
						RenderMgr->GetDevice()->SetIndices(Terrain_Info.IndexBuffer);
						RenderMgr->GetDevice()->SetFVF(VERTEX::FVF);
						/*RenderMgr->GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0,
						Terrain_Info.HeightSize * Terrain_Info.WidthSize, 0, (Terrain_Info.HeightSize - 1) * (Terrain_Info.WidthSize - 1) * 2);*/
						RenderMgr->GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0,
							Terrain_Info.HeightSize * Terrain_Info.WidthSize, 0, Terrain_Info.Triangles);
					}
					Shader_Terrain_Shadow->EndPass();
				}
			}
			Shader_Terrain_Shadow->End();
		}

	}

	void etuTerrain::MiniAlphaTextureRender()
	{
		/*int WIDTH = RenderMgr->GetWidth();
		int MINIALPHASIZE = 128;

		static float MiniAlphaTex[4][6] =
		{
			{ WIDTH - MINIALPHASIZE,			0.0f, 0.0f, 1.0f, 0.0f, 0.0f },
			{ WIDTH,						0.0f, 0.0f, 1.0f, 1.0f, 0.0f },
			{ WIDTH - MINIALPHASIZE, MINIALPHASIZE, 0.0f, 1.0f, 0.0f, 1.0f },
			{ WIDTH,			   MINIALPHASIZE, 0.0f, 1.0f, 1.0f, 1.0f },
		};

		RenderMgr->GetDevice()->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
		RenderMgr->GetDevice()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		RenderMgr->GetDevice()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		RenderMgr->GetDevice()->SetTexture(0, Terrain_Info.AlphaMap[0]);
		RenderMgr->GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, MiniAlphaTex, sizeof(MiniAlphaTex[0]));*/
	}

	void etuTerrain::SetBrsuh(bool Invert)
	{
		Brush_Info.Invert = Invert;
	}

	void etuTerrain::SetBrush(float _fTopRadiuse, float _fBottomRadiuse, float _fHeight, float _fStrong)
	{
		if (p_fBottomRadiuse > 0)
			Brush_Info.Radiuse[0] = _fBottomRadiuse;
		else
			Brush_Info.Radiuse[0] = 10.0f;

		if (p_fBottomRadiuse > _fTopRadiuse)
			Brush_Info.Radiuse[1] = _fTopRadiuse;
		else
			Brush_Info.Radiuse[1] = _fBottomRadiuse;


		if (p_fStrong > 0 && _fStrong <= 5)
			Brush_Info.Strong = _fStrong;
		else if (p_fStrong <= 0)
			Brush_Info.Strong = 0.1;
		else
			Brush_Info.Strong = 5;

		Brush_Info.Height = _fHeight;
	}

	void etuTerrain::ChanageBrushMode(int _Mode)
	{
		Brush_Info.BrushMode = _Mode;
	}

	void etuTerrain::SelectTextureIndex(int _Index)
	{
		Brush_Info.CurrentTextureIndex = _Index;
	}

	void etuTerrain::UpdateBrush(float _fTimeDelta)
	{
		if (InputMgr->GetMouseLBButtonStatus() == MOUSE_LBDOWN)
		{
			if (Brush_Info.BrushMode == TerrainBrushInfo::TEXTURE)
			{
				DrawAlphamaps(Brush_Info.CurrentTextureIndex);
			}
			else if (TerrainBrushInfo::HEIGHT_1 || Brush_Info.BrushMode == TerrainBrushInfo::HEIGHT_2)
			{
				D3DXVECTOR3 RayPosition = FindVertexUseRay(nullptr);

				for (int z = 0; z < Terrain_Info.HeightSize; z++)
				{
					for (int x = 0; x < Terrain_Info.WidthSize; x++)
					{
						int Vertex_Index = z * Terrain_Info.WidthSize + x;

						if (Brush_Info.BrushMode == TerrainBrushInfo::HEIGHT_1)
						{
							float Distance = CalculateDistance(Terrain_Info.Vertex[Vertex_Index].p, RayPosition);

							int Adjustment = Brush_Info.Radiuse[0] - Distance;

							if (Adjustment >= 0)
							{
								if(!Brush_Info.Invert)
									Terrain_Info.Vertex[Vertex_Index].p.y += Adjustment * Brush_Info.Strong * _fTimeDelta;
								else
								{
									if (Terrain_Info.Vertex[Vertex_Index].p.y > 0)
										Terrain_Info.Vertex[Vertex_Index].p.y -= Adjustment * Brush_Info.Strong * _fTimeDelta;

									if (Terrain_Info.Vertex[Vertex_Index].p.y < 0)
										Terrain_Info.Vertex[Vertex_Index].p.y = 0;
								}
							}
						}
						else if (Brush_Info.BrushMode == TerrainBrushInfo::HEIGHT_2)
						{
							RayPosition.y = 0;
							float Distance = CalculateDistance(Terrain_Info.Vertex[Vertex_Index].p, RayPosition);

							int Adjustment[2];
							Adjustment[0] = Brush_Info.Radiuse[0] - Distance;

							if (Adjustment[0] >= 0)
							{
								Adjustment[1] = Brush_Info.Radiuse[1] - Distance;

								if (Adjustment[1] >= 0)
								{
									if (Terrain_Info.Vertex[Vertex_Index].p.y >= Brush_Info.Height)
										Terrain_Info.Vertex[Vertex_Index].p.y = Brush_Info.Height;
									else
										Terrain_Info.Vertex[Vertex_Index].p.y += Brush_Info.Strong * _fTimeDelta;
								}
								else
								{
									float GabDistance = Adjustment[0] * Brush_Info.Height / (Brush_Info.Radiuse[0] - Brush_Info.Radiuse[1]);

									if (Terrain_Info.Vertex[Vertex_Index].p.y <= GabDistance)
										Terrain_Info.Vertex[Vertex_Index].p.y += Brush_Info.Strong * _fTimeDelta;
								}
							}
						}
					}
				}

				InitVertexBuffer();
				//InitIndexBuffer();
			}
		}
	}

	TerrainInfo etuTerrain::GetInfo()
	{
		return Terrain_Info;
	}

	TerrainBrushInfo etuTerrain::GetBrushInfo()
	{
		return Brush_Info;
	}

	void etuTerrain::RenderBrush()
	{
		// ���� OFF
		RenderMgr->GetDevice()->SetRenderState(D3DRS_LIGHTING, FALSE);

		// ���ؽ�
		VERTEX BrushLine[2];
		D3DXVECTOR3 CurrentPosition(1.0f, 0.0f, 0.0f);
		D3DXVECTOR3 NextPosition;
		D3DXVECTOR3 RayPosition = FindVertexUseRay(nullptr);

		if (Brush_Info.BrushMode == TerrainBrushInfo::HEIGHT_2)
			RayPosition.y = 0;

		D3DXMATRIX matRotation;

		D3DXVECTOR3 Temp;
		Temp = CurrentPosition * Brush_Info.Radiuse[0] + RayPosition;

		BrushLine[1].p.x = Temp.x;
		BrushLine[1].p.y = Temp.y + 0.5f;
		BrushLine[1].p.z = Temp.z;

		for (int i = 1; i < Brush_Info.DotCount + 1; i++)
		{
			BrushLine[0] = BrushLine[1];

			D3DXMatrixRotationY(&matRotation, i * Brush_Info.Radian);
			D3DXVec3TransformCoord(&NextPosition, &CurrentPosition, &matRotation);
			D3DXVec3Normalize(&NextPosition, &NextPosition);

			Temp = NextPosition * Brush_Info.Radiuse[0] + RayPosition;

			BrushLine[1].p.x = Temp.x;
			BrushLine[1].p.y = Temp.y + 0.5f;
			BrushLine[1].p.z = Temp.z;

			RenderMgr->GetDevice()->SetFVF(VERTEX::FVF);
			RenderMgr->GetDevice()->DrawPrimitiveUP(D3DPT_LINELIST, 1, BrushLine, sizeof(VERTEX));
		}

		if (Brush_Info.BrushMode == TerrainBrushInfo::HEIGHT_2)
		{
			// ���ؽ�
			CurrentPosition = D3DXVECTOR3(1.0f, 0.0f, 0.0f);

			Temp = CurrentPosition * Brush_Info.Radiuse[1] + RayPosition;
			Temp.y = Brush_Info.Height;
			BrushLine[1].p.x = Temp.x;
			BrushLine[1].p.y = Temp.y + 0.5f;
			BrushLine[1].p.z = Temp.z;

			for (int i = 1; i < Brush_Info.DotCount + 1; i++)
			{
				BrushLine[0] = BrushLine[1];

				D3DXMatrixRotationY(&matRotation, i * Brush_Info.Radian);
				D3DXVec3TransformCoord(&NextPosition, &CurrentPosition, &matRotation);
				D3DXVec3Normalize(&NextPosition, &NextPosition);

				Temp = NextPosition * Brush_Info.Radiuse[1] + RayPosition;
				Temp.y = Brush_Info.Height;
				BrushLine[1].p.x = Temp.x;
				BrushLine[1].p.y = Temp.y + 0.5f;
				BrushLine[1].p.z = Temp.z;

				RenderMgr->GetDevice()->SetFVF(VERTEX::FVF);
				RenderMgr->GetDevice()->DrawPrimitiveUP(D3DPT_LINELIST, 1, BrushLine, sizeof(VERTEX));
			}
		}
		RenderMgr->GetDevice()->SetRenderState(D3DRS_LIGHTING, TRUE);
	}

	int etuTerrain::GetBrushMode()
	{
		return Brush_Info.BrushMode;
	}

	void etuTerrain::Release()
	{
		//if (Terrain_Info.HeigtMapFileName != nullptr)
		//	delete[] Terrain_Info.HeigtMapFileName;

		if (Terrain_Info.Vertex != nullptr)
			delete[] Terrain_Info.Vertex;

		if (Terrain_Info.HeightMap != nullptr)
			Terrain_Info.HeightMap->Release();

		for (int i = 0; i < 5; i++)
			if (Terrain_Info.LayerMapFile[i] != nullptr)
				Terrain_Info.LayerMapFile[i]->Release();

		if (Terrain_Info.IndexBuffer != nullptr)
			Terrain_Info.IndexBuffer->Release();

		if (Terrain_Info.VertexBuffer != nullptr)
			Terrain_Info.VertexBuffer->Release();
	}
}