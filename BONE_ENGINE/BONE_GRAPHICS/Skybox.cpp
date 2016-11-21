#include "Common.h"
#include "Skybox.h"
#include "RenderManager.h"
#include "Transform3D.h"
#pragma warning(disable:4996)

namespace BONE_GRAPHICS
{
	Skybox::Skybox()
	{
		Is_Init = false;
	}

	bool Skybox::SetSkybox(char* name, string _fileType)
	{
		char TextureAddress[6][100] = { {} };

		//상자 만들기
		char temp[100] = "Resource\\Environment\\Skybox\\";
		for (int i = 0; i < 6; i++)
		{
			strcpy(TextureAddress[i], temp);
			strcat(TextureAddress[i], name);
			strcat(TextureAddress[i], "\\");
			strcat(TextureAddress[i], name);
		}

		strcat(TextureAddress[0], "_up");
		strcat(TextureAddress[0], _fileType.c_str());

		strcat(TextureAddress[1], "_dn");
		strcat(TextureAddress[1], _fileType.c_str());

		strcat(TextureAddress[2], "_lf");
		strcat(TextureAddress[2], _fileType.c_str());

		strcat(TextureAddress[3], "_rt");
		strcat(TextureAddress[3], _fileType.c_str());

		strcat(TextureAddress[4], "_ft");
		strcat(TextureAddress[4], _fileType.c_str());

		strcat(TextureAddress[5], "_bk");
		strcat(TextureAddress[5], _fileType.c_str());


		// - texture UV까지 생각해보면 정점 24개가 필요하다.
		if (FAILED(D3DXCreateTextureFromFile(RenderMgr->GetDevice(), TextureAddress[0], &m_pMeshTextures[0])))
			m_pMeshTextures[0] = NULL;
		if (FAILED(D3DXCreateTextureFromFile(RenderMgr->GetDevice(), TextureAddress[1], &m_pMeshTextures[1])))
			m_pMeshTextures[1] = NULL;
		if (FAILED(D3DXCreateTextureFromFile(RenderMgr->GetDevice(), TextureAddress[2], &m_pMeshTextures[2])))
			m_pMeshTextures[2] = NULL;
		if (FAILED(D3DXCreateTextureFromFile(RenderMgr->GetDevice(), TextureAddress[3], &m_pMeshTextures[3])))
			m_pMeshTextures[3] = NULL;
		if (FAILED(D3DXCreateTextureFromFile(RenderMgr->GetDevice(), TextureAddress[4], &m_pMeshTextures[4])))
			m_pMeshTextures[4] = NULL;
		if (FAILED(D3DXCreateTextureFromFile(RenderMgr->GetDevice(), TextureAddress[5], &m_pMeshTextures[5])))
			m_pMeshTextures[5] = NULL;

		VERTEX* pv;

		for (int i = 0; i<6; i++)
		{
			if (FAILED(RenderMgr->GetDevice()->CreateVertexBuffer(sizeof(VERTEX) * 4, 0,
				VERTEX::FVF, D3DPOOL_DEFAULT, &VB[i], NULL))) return false;
		}
		//top
		if (FAILED(VB[0]->Lock(0, 0, (void**)&pv, 0))) return false;
		SetVtx3DTexture(pv[0], D3DXVECTOR3(1.f, 1.f, -1.f), D3DXVECTOR3(1, 1, 1), 0, 1);
		SetVtx3DTexture(pv[1], D3DXVECTOR3(-1.f, 1.f, -1.f), D3DXVECTOR3(1, 1, 1), 0, 0);
		SetVtx3DTexture(pv[2], D3DXVECTOR3(1.f, 1.f, 1.f), D3DXVECTOR3(1, 1, 1), 1, 1);
		SetVtx3DTexture(pv[3], D3DXVECTOR3(-1.f, 1.f, 1.f), D3DXVECTOR3(1, 1, 1), 1, 0);
		VB[0]->Unlock();
		//bottom
		if (FAILED(VB[1]->Lock(0, 0, (void**)&pv, 0))) return false;
		SetVtx3DTexture(pv[0], D3DXVECTOR3(1.f, -1.f, 1.f), D3DXVECTOR3(1, 1, 1), 0, 0);
		SetVtx3DTexture(pv[1], D3DXVECTOR3(-1.f, -1.f, 1.f), D3DXVECTOR3(1, 1, 1), 1, 0);
		SetVtx3DTexture(pv[2], D3DXVECTOR3(1.f, -1.f, -1.f), D3DXVECTOR3(1, 1, 1), 0, 1);
		SetVtx3DTexture(pv[3], D3DXVECTOR3(-1.f, -1.f, -1.f), D3DXVECTOR3(1, 1, 1), 1, 1);
		VB[1]->Unlock();
		//left
		if (FAILED(VB[2]->Lock(0, 0, (void**)&pv, 0))) return false;
		SetVtx3DTexture(pv[0], D3DXVECTOR3(-1.f, 1.f, 1.f), D3DXVECTOR3(1, 1, 1), 0, 0);
		SetVtx3DTexture(pv[1], D3DXVECTOR3(-1.f, 1.f, -1.f), D3DXVECTOR3(1, 1, 1), 1, 0);
		SetVtx3DTexture(pv[2], D3DXVECTOR3(-1.f, -1.f, 1.f), D3DXVECTOR3(1, 1, 1), 0, 1);
		SetVtx3DTexture(pv[3], D3DXVECTOR3(-1.f, -1.f, -1.f), D3DXVECTOR3(1, 1, 1), 1, 1);
		VB[2]->Unlock();
		//right
		if (FAILED(VB[3]->Lock(0, 0, (void**)&pv, 0))) return false;
		SetVtx3DTexture(pv[0], D3DXVECTOR3(1.f, 1.f, -1.f), D3DXVECTOR3(1, 1, 1), 0, 0);
		SetVtx3DTexture(pv[1], D3DXVECTOR3(1.f, 1.f, 1.f), D3DXVECTOR3(1, 1, 1), 1, 0);
		SetVtx3DTexture(pv[2], D3DXVECTOR3(1.f, -1.f, -1.f), D3DXVECTOR3(1, 1, 1), 0, 1);
		SetVtx3DTexture(pv[3], D3DXVECTOR3(1.f, -1.f, 1.f), D3DXVECTOR3(1, 1, 1), 1, 1);
		VB[3]->Unlock();
		//front
		if (FAILED(VB[4]->Lock(0, 0, (void**)&pv, 0))) return false;
		SetVtx3DTexture(pv[0], D3DXVECTOR3(1.f, 1.f, 1.f), D3DXVECTOR3(1, 1, 1), 0, 0);
		SetVtx3DTexture(pv[1], D3DXVECTOR3(-1.f, 1.f, 1.f), D3DXVECTOR3(1, 1, 1), 1, 0);
		SetVtx3DTexture(pv[2], D3DXVECTOR3(1.f, -1.f, 1.f), D3DXVECTOR3(1, 1, 1), 0, 1);
		SetVtx3DTexture(pv[3], D3DXVECTOR3(-1.f, -1.f, 1.f), D3DXVECTOR3(1, 1, 1), 1, 1);
		VB[4]->Unlock();
		//back
		if (FAILED(VB[5]->Lock(0, 0, (void**)&pv, 0))) return false;
		SetVtx3DTexture(pv[0], D3DXVECTOR3(-1.f, 1.f, -1.f), D3DXVECTOR3(1, 1, 1), 0, 0);
		SetVtx3DTexture(pv[1], D3DXVECTOR3(1.f, 1.f, -1.f), D3DXVECTOR3(1, 1, 1), 1, 0);
		SetVtx3DTexture(pv[2], D3DXVECTOR3(-1.f, -1.f, -1.f), D3DXVECTOR3(1, 1, 1), 0, 1);
		SetVtx3DTexture(pv[3], D3DXVECTOR3(1.f, -1.f, -1.f), D3DXVECTOR3(1, 1, 1), 1, 1);
		VB[5]->Unlock();
		for (int i = 0; i<6; i++)
		{
			//인덱스 버퍼
			if (FAILED(RenderMgr->GetDevice()->CreateIndexBuffer(sizeof(VERTEX_INDEX) * 12, 0,
				D3DFMT_INDEX16, D3DPOOL_DEFAULT, &IB[i], NULL))) return false;
		}
		//인덱스 버퍼 초기화
		VERTEX_INDEX* iv;
		for (int i = 0; i<6; i++)
		{
			if (FAILED(IB[i]->Lock(0, 0, (void**)&iv, 0))) return false;
			SetIdx(iv[0], 0, 1, 2);
			SetIdx(iv[1], 2, 1, 3);
			IB[i]->Unlock();
		}

		for (int i = 0; i < 6; i++)
		{
			if (!m_pMeshTextures[i])
			{
				return false;
			}
		}

		Is_Init = true;

		return Is_Init;
	}

	void Skybox::Render(GameObject* _cameraObject)
	{
		if (Is_Init)
		{
			RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			RenderMgr->GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			RenderMgr->GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
			RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
			RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHAREF, 0x08);
			RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
			RenderMgr->GetDevice()->SetSamplerState(7, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
			RenderMgr->GetDevice()->SetSamplerState(7, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);

			if (VB != NULL && IB != NULL)
			{

				//스카이 박스를 그린후 월드 행렬을 다시 초기화 시킴.
				RenderMgr->GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);	//컬링은 양면
				RenderMgr->GetDevice()->SetRenderState(D3DRS_ZENABLE, FALSE);			//Z버퍼 끄기
				RenderMgr->GetDevice()->SetRenderState(D3DRS_LIGHTING, FALSE);			//조명 끄기

																						//스카이 박스 이동.
				D3DXVECTOR3 vPos = ((Transform3D*)_cameraObject->GetComponent("Transform3D"))->GetPosition();

				D3DXMatrixIdentity(&matWrd);
				D3DXMatrixTranslation(&matWrd, vPos.x, vPos.y, vPos.z);
				RenderMgr->GetDevice()->SetTransform(D3DTS_WORLD, &matWrd);

				//텍스쳐 UV Address를 설정.
				//RenderMgr->GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
				//RenderMgr->GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

				//스카이박스 드로우.
				for (int i = 0; i < 6; i++)
				{
					RenderMgr->GetDevice()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
					RenderMgr->GetDevice()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);

					RenderMgr->GetDevice()->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
					RenderMgr->GetDevice()->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
					RenderMgr->GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSU, NULL);
					RenderMgr->GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSV, NULL);

					RenderMgr->GetDevice()->SetTexture(0, (m_pMeshTextures[i]) ? m_pMeshTextures[i] : NULL);
					RenderMgr->GetDevice()->SetStreamSource(0, VB[i], 0, sizeof(VERTEX));
					RenderMgr->GetDevice()->SetFVF(VERTEX::FVF);
					RenderMgr->GetDevice()->SetIndices(IB[i]);
					RenderMgr->GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);
				}

				//행렬 초기화.
				RenderMgr->GetDevice()->SetRenderState(D3DRS_ZENABLE, TRUE);			//Z버퍼 켜기
				RenderMgr->GetDevice()->SetRenderState(D3DRS_LIGHTING, TRUE);			//조명 켜기
				RenderMgr->GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);	//컬링 반시계방향

			}

			RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE); // disable alpha blend
			RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE); // i am not sure maybie this one needs to be disabled also
		}
	}

	void Skybox::Release()
	{
		for (int i = 0; i<6; i++)
		{
			if (m_pMeshTextures[i] != NULL)
				m_pMeshTextures[i]->Release();

			if (VB[i] != NULL)
				VB[i]->Release();

			if (IB[i] != NULL)
				IB[i]->Release();
		}
	}
}