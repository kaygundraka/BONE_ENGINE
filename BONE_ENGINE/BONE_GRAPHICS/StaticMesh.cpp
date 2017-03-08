#include "Common.h"
#include "RenderManager.h"
#include "LogManager.h"
#include "ResourceManager.h"
#include "StaticMesh.h"
#include <shlwapi.h>
#include "GameObject.h"
#pragma comment(lib, "shlwapi")
#pragma warning(disable:4996)

namespace BONE_GRAPHICS
{
	StaticMesh::StaticMesh()
	{
		SetTypeName("StaticMesh");

		renderMode = RENDER_DEFAULT;
	}

	StaticMesh::~StaticMesh()
	{
		ResourceMgr->ReleaseMesh(address);

		for (int i = 0; i < ResourceMgr->FindMesh(address)->numMaterials; i++)
			ResourceMgr->ReleaseTexture(textureAddress[i]);
	}

	void StaticMesh::LoadContent()
	{
		char Address[MAX_PATH];
		strcpy(Address, address.c_str());

		try {
			if (IsInit)
			{
				meshInfo = ResourceMgr->LoadMesh(address);

				if(meshInfo == nullptr)
				{
					LogMgr->ShowMessage(LOG_ERROR, "%s : Could Not Find Xfile.", Address);
					throw;
				}

				int MaterialSize = meshInfo->numMaterials;

				textureAddress = new string[MaterialSize];

				D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)meshInfo->buffer->GetBufferPointer();

				meshMaterials = new D3DMATERIAL9[MaterialSize];

				for (int i = 0; i < MaterialSize; i++)
				{
					// 재질 정보 복사
					meshMaterials[i] = d3dxMaterials[i].MatD3D;

					// 재질용 주변광 색깔 설정(D3DX에서 직접해주지 않음)
					meshMaterials[i].Ambient = meshMaterials[i].Diffuse;

					char TexAddr[100];
					strcpy(TexAddr, Address);
					int TexLen = strlen(TexAddr);
					char Temp;

					while (1)
					{
						Temp = TexAddr[TexLen];

						if (Temp != '\\')
						{
							TexAddr[TexLen--] = nullptr;
						}
						else
							break;
					}

					if (d3dxMaterials[i].pTextureFilename != nullptr)
					{
						char* filename = PathFindFileName(d3dxMaterials[i].pTextureFilename);

						strcat(TexAddr, filename);
					}

					if (d3dxMaterials[i].pTextureFilename != nullptr && lstrlenA(d3dxMaterials[i].pTextureFilename) > 0)
					{
						textureAddress[i] = TexAddr;

						// 텍스쳐 생성
						if (ResourceMgr->LoadTexture(TexAddr) == nullptr)
						{
							LogMgr->ShowMessage(LOG_ERROR, "%s : Could Not Find TextureFile.", TexAddr);
							throw;
						}
					}
				}

				// 재질 버퍼 사용 완료;
				//(D3DXMATERIAL*)meshInfo->_buffer->Release();

				IsInit = true;
			}
		}
		catch (...)
		{
			IsInit = false;
		}
	}

	bool StaticMesh::CheckMouseRayInMesh(Transform3D* tr)
	{
		RAY ray = RenderMgr->GetPickingRayToView(false);

		bool Result = RenderMgr->CheckRayInMesh(&ray, tr->GetTransform(), ResourceMgr->FindMesh(address)->mesh, nullptr);

		return Result;
	}

	void StaticMesh::SetRenderMode(int mode)
	{
		renderMode = mode;
	}

	string* StaticMesh::GetTexturesAddress()
	{
		return textureAddress;
	}

	void StaticMesh::SetTexturesAddress(string* address)
	{
		textureAddress = address;
	}

	void StaticMesh::Render(IShader* shaderOpt, GameObject* object)
	{
		if (IsInit)
		{
			Matrix Transform = ((Transform3D*)_object->GetComponent("Transform3D"))->GetTransform();
			RenderMgr->GetDevice()->SetTransform(D3DTS_WORLD, &Transform);

			if (renderMode == RENDER_ALPHA)
			{
				RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
				RenderMgr->GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHAREF, 0);

				RenderMgr->GetDevice()->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTA_TEXTURE);
			}
			else if (renderMode == RENDER_STENCIL)
			{
				RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
				RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHAREF, 0x00);
				RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
			}

			if (shaderOpt == nullptr)
			{

				for (int i = 0; i < ResourceMgr->FindMesh(address)->numMaterials; i++)
				{
					RenderMgr->GetDevice()->SetMaterial(&meshMaterials[i]);
					RenderMgr->GetDevice()->SetTexture(0, ResourceMgr->LoadTexture(textureAddress[i]));
					ResourceMgr->FindMesh(address)->mesh->DrawSubset(i);
				}

			}
			else
			{
				for (int i = 0; i < ResourceMgr->FindMesh(address)->numMaterials; i++)
				{
					shaderOpt->Render(i, _object);
					ResourceMgr->FindMesh(address)->mesh->DrawSubset(i);
				}
			}

			if (renderMode == RENDER_ALPHA)
				RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
			else if (renderMode == RENDER_STENCIL)
				RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, false);
		}
	}

	void StaticMesh::SetFileAddress(string address)
	{
		address = address;
	}

	string StaticMesh::GetFileAddress()
	{
		return address;
	}
}