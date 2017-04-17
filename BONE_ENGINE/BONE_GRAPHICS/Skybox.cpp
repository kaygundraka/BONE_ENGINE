#include "Common.h"
#include "Skybox.h"
#include "RenderManager.h"
#include "ResourceManager.h"
#include "Transform3D.h"
#include "SceneManager.h"

namespace BONE_GRAPHICS
{
	Skybox::Skybox()
	{
        folder = "None";
		Is_Init = false;
	}

	bool Skybox::SetSkybox(char* name, std::string fileType)
	{
		char TextureAddress[6][100] = { {} };
        folder = name;
        this->fileType = fileType;

        std::string FullPath;
        ResourceMgr->ExistFile(name, &FullPath);

        for (int i = 0; i < 6; i++)
		{
			strcpy_s(TextureAddress[i], FullPath.c_str());
			strcat_s(TextureAddress[i], "\\");
			strcat_s(TextureAddress[i], name);
		}

		strcat_s(TextureAddress[0], "_up.");
		strcat_s(TextureAddress[0], fileType.c_str());

		strcat_s(TextureAddress[1], "_dn.");
		strcat_s(TextureAddress[1], fileType.c_str());

		strcat_s(TextureAddress[2], "_lf.");
		strcat_s(TextureAddress[2], fileType.c_str());

		strcat_s(TextureAddress[3], "_rt.");
		strcat_s(TextureAddress[3], fileType.c_str());

		strcat_s(TextureAddress[4], "_ft.");
		strcat_s(TextureAddress[4], fileType.c_str());

		strcat_s(TextureAddress[5], "_bk.");
		strcat_s(TextureAddress[5], fileType.c_str());

		// - texture UV까지 생각해보면 정점 24개가 필요하다.
		if (FAILED(D3DXCreateTextureFromFile(RenderMgr->GetDevice(), TextureAddress[0], &meshTextures[0])))
            meshTextures[0] = nullptr;
		if (FAILED(D3DXCreateTextureFromFile(RenderMgr->GetDevice(), TextureAddress[1], &meshTextures[1])))
            meshTextures[1] = nullptr;
		if (FAILED(D3DXCreateTextureFromFile(RenderMgr->GetDevice(), TextureAddress[2], &meshTextures[2])))
            meshTextures[2] = nullptr;
		if (FAILED(D3DXCreateTextureFromFile(RenderMgr->GetDevice(), TextureAddress[3], &meshTextures[3])))
            meshTextures[3] = nullptr;
		if (FAILED(D3DXCreateTextureFromFile(RenderMgr->GetDevice(), TextureAddress[4], &meshTextures[4])))
            meshTextures[4] = nullptr;
		if (FAILED(D3DXCreateTextureFromFile(RenderMgr->GetDevice(), TextureAddress[5], &meshTextures[5])))
            meshTextures[5] = nullptr;

		VERTEX* pv;

		for (int i = 0; i<6; i++)
			if (FAILED(RenderMgr->GetDevice()->CreateVertexBuffer(sizeof(VERTEX) * 4, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
				VERTEX::FVF, D3DPOOL_DEFAULT, &VB[i], nullptr))) return false;
		
        //top
		if (FAILED(VB[0]->Lock(0, 0, (void**)&pv, D3DLOCK_NOSYSLOCK | D3DLOCK_DISCARD))) return false;
		SetVtx3DTexture(pv[0], D3DXVECTOR3(1.f, 1.f, -1.f), D3DXVECTOR3(1, 1, 1), 0, 1);
		SetVtx3DTexture(pv[1], D3DXVECTOR3(-1.f, 1.f, -1.f), D3DXVECTOR3(1, 1, 1), 0, 0);
		SetVtx3DTexture(pv[2], D3DXVECTOR3(1.f, 1.f, 1.f), D3DXVECTOR3(1, 1, 1), 1, 1);
		SetVtx3DTexture(pv[3], D3DXVECTOR3(-1.f, 1.f, 1.f), D3DXVECTOR3(1, 1, 1), 1, 0);
		VB[0]->Unlock();
		//bottom
		if (FAILED(VB[1]->Lock(0, 0, (void**)&pv, D3DLOCK_NOSYSLOCK | D3DLOCK_DISCARD))) return false;
		SetVtx3DTexture(pv[0], D3DXVECTOR3(1.f, -1.f, 1.f), D3DXVECTOR3(1, 1, 1), 0, 0);
		SetVtx3DTexture(pv[1], D3DXVECTOR3(-1.f, -1.f, 1.f), D3DXVECTOR3(1, 1, 1), 1, 0);
		SetVtx3DTexture(pv[2], D3DXVECTOR3(1.f, -1.f, -1.f), D3DXVECTOR3(1, 1, 1), 0, 1);
		SetVtx3DTexture(pv[3], D3DXVECTOR3(-1.f, -1.f, -1.f), D3DXVECTOR3(1, 1, 1), 1, 1);
		VB[1]->Unlock();
		//left
		if (FAILED(VB[2]->Lock(0, 0, (void**)&pv, D3DLOCK_NOSYSLOCK | D3DLOCK_DISCARD))) return false;
		SetVtx3DTexture(pv[0], D3DXVECTOR3(-1.f, 1.f, 1.f), D3DXVECTOR3(1, 1, 1), 0, 0);
		SetVtx3DTexture(pv[1], D3DXVECTOR3(-1.f, 1.f, -1.f), D3DXVECTOR3(1, 1, 1), 1, 0);
		SetVtx3DTexture(pv[2], D3DXVECTOR3(-1.f, -1.f, 1.f), D3DXVECTOR3(1, 1, 1), 0, 1);
		SetVtx3DTexture(pv[3], D3DXVECTOR3(-1.f, -1.f, -1.f), D3DXVECTOR3(1, 1, 1), 1, 1);
		VB[2]->Unlock();
		//right
		if (FAILED(VB[3]->Lock(0, 0, (void**)&pv, D3DLOCK_NOSYSLOCK | D3DLOCK_DISCARD))) return false;
		SetVtx3DTexture(pv[0], D3DXVECTOR3(1.f, 1.f, -1.f), D3DXVECTOR3(1, 1, 1), 0, 0);
		SetVtx3DTexture(pv[1], D3DXVECTOR3(1.f, 1.f, 1.f), D3DXVECTOR3(1, 1, 1), 1, 0);
		SetVtx3DTexture(pv[2], D3DXVECTOR3(1.f, -1.f, -1.f), D3DXVECTOR3(1, 1, 1), 0, 1);
		SetVtx3DTexture(pv[3], D3DXVECTOR3(1.f, -1.f, 1.f), D3DXVECTOR3(1, 1, 1), 1, 1);
		VB[3]->Unlock();
		//front
		if (FAILED(VB[4]->Lock(0, 0, (void**)&pv, D3DLOCK_NOSYSLOCK | D3DLOCK_DISCARD))) return false;
		SetVtx3DTexture(pv[0], D3DXVECTOR3(1.f, 1.f, 1.f), D3DXVECTOR3(1, 1, 1), 0, 0);
		SetVtx3DTexture(pv[1], D3DXVECTOR3(-1.f, 1.f, 1.f), D3DXVECTOR3(1, 1, 1), 1, 0);
		SetVtx3DTexture(pv[2], D3DXVECTOR3(1.f, -1.f, 1.f), D3DXVECTOR3(1, 1, 1), 0, 1);
		SetVtx3DTexture(pv[3], D3DXVECTOR3(-1.f, -1.f, 1.f), D3DXVECTOR3(1, 1, 1), 1, 1);
		VB[4]->Unlock();
		//back
		if (FAILED(VB[5]->Lock(0, 0, (void**)&pv, D3DLOCK_NOSYSLOCK | D3DLOCK_DISCARD))) return false;
		SetVtx3DTexture(pv[0], D3DXVECTOR3(-1.f, 1.f, -1.f), D3DXVECTOR3(1, 1, 1), 0, 0);
		SetVtx3DTexture(pv[1], D3DXVECTOR3(1.f, 1.f, -1.f), D3DXVECTOR3(1, 1, 1), 1, 0);
		SetVtx3DTexture(pv[2], D3DXVECTOR3(-1.f, -1.f, -1.f), D3DXVECTOR3(1, 1, 1), 0, 1);
		SetVtx3DTexture(pv[3], D3DXVECTOR3(1.f, -1.f, -1.f), D3DXVECTOR3(1, 1, 1), 1, 1);
		VB[5]->Unlock();
		for (int i = 0; i<6; i++)
			if (FAILED(RenderMgr->GetDevice()->CreateIndexBuffer(sizeof(VERTEX_INDEX) * 12, 0,
				D3DFMT_INDEX16, D3DPOOL_DEFAULT, &IB[i], nullptr))) return false;
		
        //인덱스 버퍼 초기화
		VERTEX_INDEX* iv;
		for (int i = 0; i<6; i++)
		{
			if (FAILED(IB[i]->Lock(0, 0, (void**)&iv, D3DLOCK_NOSYSLOCK | D3DLOCK_DISCARD))) return false;
			SetIdx(iv[0], 0, 1, 2);
			SetIdx(iv[1], 2, 1, 3);
			IB[i]->Unlock();
		}

		for (int i = 0; i < 6; i++)
			if (!meshTextures[i])
				return false;
		
		Is_Init = true;

		return Is_Init;
	}

    std::string Skybox::GetFolderName()
    {
        return folder;
    }

    std::string Skybox::GetFileType()
    {
        return fileType;
    }

	void Skybox::Render()
	{
		if (Is_Init)
		{
            GameObject* cameraObject = CUR_SCENE->GetCurrentCamera();

            D3DXVECTOR3 vPos = ((Transform3D*)cameraObject->GetComponent("Transform3D"))->GetPosition();

            D3DXMatrixIdentity(&matWrd);
            D3DXMatrixTranslation(&matWrd, vPos.x, vPos.y, vPos.z);

			RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			RenderMgr->GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			RenderMgr->GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
			RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
			RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHAREF, 0x08);
			RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
			RenderMgr->GetDevice()->SetSamplerState(7, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
			RenderMgr->GetDevice()->SetSamplerState(7, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);

			if (VB != nullptr && IB != nullptr)
			{
				//스카이 박스를 그린후 월드 행렬을 다시 초기화 시킴.
				RenderMgr->GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);	//컬링은 양면
				RenderMgr->GetDevice()->SetRenderState(D3DRS_ZENABLE, FALSE);			//Z버퍼 끄기
				RenderMgr->GetDevice()->SetRenderState(D3DRS_LIGHTING, FALSE);			//조명 끄기

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
					RenderMgr->GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSU, 0);
					RenderMgr->GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSV, 0);

					RenderMgr->GetDevice()->SetTexture(0, (meshTextures[i]) ? meshTextures[i] : nullptr);
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
			if (meshTextures[i] != nullptr)
                meshTextures[i]->Release();

			if (VB[i] != nullptr)
				VB[i]->Release();

			if (IB[i] != nullptr)
				IB[i]->Release();
		}
	}
}