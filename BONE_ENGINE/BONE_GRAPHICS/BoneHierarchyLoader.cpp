#include "Common.h"
#include "RenderManager.h"
#include "BoneHierarchyLoader.h"
#pragma warning(disable:4996)

namespace BONE_GRAPHICS
{
	HRESULT BoneHierarchyLoader::CreateFrame(LPCTSTR Name, LPD3DXFRAME *ppNewFrame)
	{
		Bone *newBone = new Bone;
		memset(newBone, 0, sizeof(Bone));

		// Name ����
		if (Name != nullptr)
		{
			newBone->Name = new char[strlen(Name) + 1];
			strcpy(newBone->Name, Name);
		}

		// ��ȯ ��ĵ��� �ʱ�ȭ.
		// D3DXMatrixIdentity = ������� ����� �Լ�
		D3DXMatrixIdentity(&newBone->TransformationMatrix);
		D3DXMatrixIdentity(&newBone->CombinedTransformationMatrix);

		// ������ ���� �����Ѵ�.
		*ppNewFrame = (D3DXFRAME*)newBone;

		return S_OK;
	}

	HRESULT BoneHierarchyLoader::DestroyFrame(LPD3DXFRAME pFrameToFree)
	{
		if (pFrameToFree)
		{
			// Free Name String
			if (pFrameToFree->Name != nullptr)
				delete[] pFrameToFree->Name;

			// Free Frame
			delete pFrameToFree;
		}

		pFrameToFree = nullptr;

		return S_OK;
	}

	HRESULT BoneHierarchyLoader::CreateMeshContainer(LPCSTR Name,
		CONST D3DXMESHDATA *pMeshData,
		CONST D3DXMATERIAL *pMaterials,
		CONST D3DXEFFECTINSTANCE *pEffectInstance,
		int NumMaterials,
		CONST int *pAdjacency,
		LPD3DXSKININFO pSkinInfo,
		LPD3DXMESHCONTAINER *ppNewMeshContainer)
	{
		// �� BoneMesh�� �����.
		BoneMesh *boneMesh = new BoneMesh;
		memset(boneMesh, 0, sizeof(BoneMesh));

		// �޽� �����͸� ��´�.
		// ����Ʈ���� ��Ű���� ���ؼ��� �޽��� ���� ������ �����͸� ������ �ξ�� �Ѵ�.
		boneMesh->OriginalMesh = pMeshData->pMesh;
		boneMesh->MeshData.pMesh = pMeshData->pMesh;
		boneMesh->MeshData.Type = pMeshData->Type;

		// �޽��� �޸𸮰� �������� �ʵ��� �������� ������Ų��.
		pMeshData->pMesh->AddRef();

		// D3D ����̽� �����͸� ��´�.
		IDirect3DDevice9 *pDevice = nullptr;//RenderMgr->GetDevice();
		pMeshData->pMesh->GetDevice(&pDevice);

		// ������ �����ϰ� �ؽ�ó�� �ε��Ѵ�. (����ƽ �޽��� ���� �����ϴ�.)
		for (UINT i = 0; i < NumMaterials; i++)
		{
			D3DXMATERIAL mtrl;
			memcpy(&mtrl, &pMaterials[i], sizeof(D3DXMATERIAL));
			boneMesh->materials.push_back(mtrl.MatD3D);

			IDirect3DTexture9* newTexture = nullptr;

			if (mtrl.pTextureFilename != nullptr)
			{
				char textureFileName[200];
				strcpy(textureFileName, "Resource\\Meshbank\\");

				char* TextureFileRealName;
				int Vertex_Index = strlen(mtrl.pTextureFilename);

				char Temp[20] = "";
				int Index2 = 0;

				while (1)
				{
					if (mtrl.pTextureFilename[Vertex_Index] != '\\' && mtrl.pTextureFilename[Vertex_Index] != '/' && Vertex_Index >= 0)
					{
						Temp[Index2++] = mtrl.pTextureFilename[Vertex_Index--];
					}
					else
					{
						TextureFileRealName = new char[Index2 + 1];

						for (int i = 0; i < Index2; i++)
							TextureFileRealName[i] = Temp[Index2 - i - 1];

						break;
					}
				}

				strcat(textureFileName, TextureFileRealName);

				delete[] TextureFileRealName;
				// �ؽ�ó �ε�
				D3DXCreateTextureFromFile(RenderMgr->GetDevice(), textureFileName, &newTexture);

			}
			boneMesh->textures.push_back(newTexture);
		}

		// ��Ų ������ ���޵Ǿ��ٸ� �̸� �����ؾ��Ѵ�.
		if (pSkinInfo != nullptr)
		{
			// ��Ų ������ ��´�.
			boneMesh->pSkinInfo = pSkinInfo;

			// SkinInfo�� �������� �ʵ��� ���� ���� ������Ų��.
			pSkinInfo->AddRef();

			// �޽ø� �ε��� ����� �޽÷� ��ȯ�ϰ�, �� �ĺ���̼� ���̺��� �����صд�
			/*int maxVertInfluences = 0;
			int numBoneComboEntries = 0;

			pSkinInfo->ConvertToIndexedBlendedMesh(pMeshData->pMesh,
			D3DXMESH_MANAGED | D3DXMESH_WRITEONLY,
			30,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			&maxVertInfluences,
			&numBoneComboEntries,
			&boneMesh->pBoneCombinationBuf,
			&boneMesh->MeshData.pMesh);*/

			// �޽ø� ������ boneMesh->MeshData->pMesh�� �����Ѵ�.
			// �� ������ �޽ô� ���߿� ������ ������ �Ǵ� ��Ų�� �޽ð� �ȴ�.
			pMeshData->pMesh->CloneMeshFVF(D3DXMESH_MANAGED,
				pMeshData->pMesh->GetFVF(),
				RenderMgr->GetDevice(),
				&boneMesh->MeshData.pMesh);

			// �Ӽ� ���̺��� ��´�.
			boneMesh->MeshData.pMesh->GetAttributeTable(nullptr, &boneMesh->NumAttributeGroups);
			boneMesh->attributeTables = new D3DXATTRIBUTERANGE[boneMesh->NumAttributeGroups];
			boneMesh->MeshData.pMesh->GetAttributeTable(boneMesh->attributeTables, nullptr);

			// ���� �����°� �ջ�� ��ȯ ��ĵ��� �����.
			int NumBones = pSkinInfo->GetNumBones();
			boneMesh->boneOffsetMatrices = new D3DXMATRIX[NumBones];
			boneMesh->currentBoneMatrices = new D3DXMATRIX[NumBones];

			// �� ������ ��ĵ��� ��´�.
			for (int i = 0; i < NumBones; i++)
				boneMesh->boneOffsetMatrices[i] = *(boneMesh->pSkinInfo->GetBoneOffsetMatrix(i));
		}

		// ������ ������ boneMesh�� ��ȯ�Ѵ�.
		*ppNewMeshContainer = boneMesh;

		return S_OK;
	}

	HRESULT BoneHierarchyLoader::DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase)
	{
		BoneMesh *boneMesh = (BoneMesh*)pMeshContainerBase;

		if (boneMesh->pBoneCombinationBuf)
			boneMesh->pBoneCombinationBuf->Release();

		if (pMeshContainerBase != nullptr)
			delete pMeshContainerBase;

		return S_OK;
	}
}