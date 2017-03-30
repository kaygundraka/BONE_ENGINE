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
        show = true;
	}

	StaticMesh::~StaticMesh()
	{
		ResourceMgr->ReleaseMesh(fileName);

        if (ResourceMgr->FindMesh(fileName) != nullptr)
            for (int i = 0; i < ResourceMgr->FindMesh(fileName)->numMaterials; i++)
            {
                ResourceMgr->ReleaseTexture(textureFiles[i]);
                ResourceMgr->ReleaseTexture(normalTextureFiles[i]);
            }
        }

	void StaticMesh::LoadContent()
	{
		char Address[MAX_PATH];
		strcpy_s(Address, fileName.c_str());

		try {
            meshInfo = ResourceMgr->LoadMesh(fileName);

            if (meshInfo == nullptr)
            {
                LogMgr->Error("%s : Could Not Find Xfile.", Address);
                throw;
            }

            int MaterialSize = meshInfo->numMaterials;

            textureFiles = new string[MaterialSize];
            normalTextureFiles = new string[MaterialSize];

            D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)meshInfo->buffer->GetBufferPointer();

            meshMaterials = new D3DMATERIAL9[MaterialSize];

            for (int i = 0; i < MaterialSize; i++)
            {
                // 재질 정보 복사
                meshMaterials[i] = d3dxMaterials[i].MatD3D;

                // 재질용 주변광 색깔 설정(D3DX에서 직접해주지 않음)
                meshMaterials[i].Ambient = meshMaterials[i].Diffuse;

                char* file = PathFindFileName(d3dxMaterials[i].pTextureFilename);

                if (d3dxMaterials[i].pTextureFilename != nullptr && lstrlenA(d3dxMaterials[i].pTextureFilename) > 0)
                {
                    textureFiles[i] = file;

                    // 텍스쳐 생성
                    if (ResourceMgr->LoadTexture(file) == nullptr)
                    {
                        LogMgr->Error("%s : Could Not Find TextureFile.", file);
                        throw;
                    }

                    std::string normalMap = d3dxMaterials[i].pTextureFilename;
                    std::string file = "";

                    for (int i = 0; i < 4; i++)
                    {
                        file = normalMap.back() + file;
                        normalMap.pop_back();
                    }

                    normalMap += "_N" + file;
                    normalTextureFiles[i] = normalMap;
                }
            }

            IsInit = true;
		}
		catch (...)
		{
			IsInit = false;
		}
	}

	bool StaticMesh::CheckMouseRayInMesh(Transform3D* tr, float* dist)
	{
 		bool Result = RenderMgr->CheckRayInMesh(tr->GetTransform(), ResourceMgr->FindMesh(fileName)->mesh, dist);

		return Result;
	}

	void StaticMesh::SetRenderMode(int mode)
	{
		renderMode = mode;
	}

	string* StaticMesh::GetTextures()
	{
		return textureFiles;
	}

    string* StaticMesh::GetNormalTextures()
    {
        return normalTextureFiles;
    }

	void StaticMesh::SetTextures(string* fileNames)
	{
		textureFiles = fileNames;
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
				for (int i = 0; i < ResourceMgr->FindMesh(fileName)->numMaterials; i++)
				{
					RenderMgr->GetDevice()->SetMaterial(&meshMaterials[i]);
					RenderMgr->GetDevice()->SetTexture(0, ResourceMgr->LoadTexture(textureFiles[i]));
					ResourceMgr->FindMesh(fileName)->mesh->DrawSubset(i);
				}

			}
			else
			{
				for (int i = 0; i < ResourceMgr->FindMesh(fileName)->numMaterials; i++)
				{
					shaderOpt->Render(i, object);
					ResourceMgr->FindMesh(fileName)->mesh->DrawSubset(i);
				}
			}

			if (renderMode == RENDER_ALPHA)
				RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
			else if (renderMode == RENDER_STENCIL)
				RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, false);
        }
	}

	void StaticMesh::SetFile(string fileName)
	{
		this->fileName = fileName;
	}

	string StaticMesh::GetFile()
	{
		return fileName;
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