#include "Common.h"
#include "RenderManager.h"
#include "LogManager.h"
#include "ResourceManager.h"
#include "StaticMesh.h"
#include "GameObject.h"

namespace BONE_GRAPHICS
{
	StaticMesh::StaticMesh()
	{
		SetTypeName("StaticMesh");

		renderMode = RENDER_DEFAULT;
        showBox = false;
        show = true;

        color = COLOR::WHITE;
	}

	StaticMesh::~StaticMesh()
	{
		ResourceMgr->ReleaseMesh(address);

        if (ResourceMgr->FindMesh(address) != nullptr)
    		for (int i = 0; i < ResourceMgr->FindMesh(address)->numMaterials; i++)
	    		ResourceMgr->ReleaseTexture(textureAddress[i]);
	}

	void StaticMesh::LoadContent()
	{
		char Address[MAX_PATH];
		strcpy_s(Address, address.c_str());

		try {
            meshInfo = ResourceMgr->LoadMesh(address);

            if (meshInfo == nullptr)
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
                // ���� ���� ����
                meshMaterials[i] = d3dxMaterials[i].MatD3D;

                // ������ �ֺ��� ���� ����(D3DX���� ���������� ����)
                meshMaterials[i].Ambient = meshMaterials[i].Diffuse;

                char* filename = PathFindFileName(d3dxMaterials[i].pTextureFilename);

                if (d3dxMaterials[i].pTextureFilename != nullptr && lstrlenA(d3dxMaterials[i].pTextureFilename) > 0)
                {
                    textureAddress[i] = filename;

                    // �ؽ��� ����
                    if (ResourceMgr->LoadTexture(filename) == nullptr)
                    {
                        LogMgr->ShowMessage(LOG_ERROR, "%s : Could Not Find TextureFile.", filename);
                        throw;
                    }
                }
            }

            // ���� ���� ��� �Ϸ�;
            //(D3DXMATERIAL*)meshInfo->_buffer->Release();

            IsInit = true;
		}
		catch (...)
		{
			IsInit = false;
		}
	}

	bool StaticMesh::CheckMouseRayInMesh(Transform3D* tr, float* dist)
	{
 		bool Result = RenderMgr->CheckRayInMesh(tr->GetTransform(), ResourceMgr->FindMesh(address)->mesh, dist);

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

    void StaticMesh::ShowMeshBox(bool show)
    {
        this->showBox = show;
    }

    void StaticMesh::SetMeshBoxColor(D3DXCOLOR color)
    {
        this->color = color;
    }

    bool StaticMesh::IsShowMeshBOx()
    {
        return showBox;
    }

	void StaticMesh::Render(IShader* shaderOpt, GameObject* object)
	{
		if (IsInit && show)
		{
			Matrix Transform = ((Transform3D*)object->GetComponent("Transform3D"))->GetTransform();
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
				RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, true);
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
					shaderOpt->Render(i, object);
					ResourceMgr->FindMesh(address)->mesh->DrawSubset(i);
				}
			}

			if (renderMode == RENDER_ALPHA)
				RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
			else if (renderMode == RENDER_STENCIL)
				RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, false);
		
            if (showBox)
                RenderMgr->DrawMeshBox(ResourceMgr->FindMesh(address)->mesh, ((Transform3D*)object->GetComponent("Transform3D"))->GetPosition(), color);
        }
	}

	void StaticMesh::SetFileAddress(string address)
	{
		this->address = address;
	}

	string StaticMesh::GetFileAddress()
	{
		return address;
	}

    void StaticMesh::Hide()
    {
        show = false;
    }

    void StaticMesh::Show()
    {
        show = true;
    }
    
    bool StaticMesh::IsShow()
    {
        return show;
    }
}