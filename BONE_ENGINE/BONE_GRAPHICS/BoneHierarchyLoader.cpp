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

		// Name 복사
		if (Name != nullptr)
		{
			newBone->Name = new char[strlen(Name) + 1];
			strcpy(newBone->Name, Name);
		}

		// 변환 행렬들의 초기화.
		// D3DXMatrixIdentity = 단위행렬 만드는 함수
		D3DXMatrixIdentity(&newBone->TransformationMatrix);
		D3DXMatrixIdentity(&newBone->CombinedTransformationMatrix);

		// 생성된 본을 리턴한다.
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
		// 새 BoneMesh를 만든다.
		BoneMesh *boneMesh = new BoneMesh;
		memset(boneMesh, 0, sizeof(BoneMesh));

		// 메시 데이터를 얻는다.
		// 소프트웨어 스키닝을 위해서는 메시의 원본 형태의 데이터를 보관해 두어야 한다.
		boneMesh->OriginalMesh = pMeshData->pMesh;
		boneMesh->MeshData.pMesh = pMeshData->pMesh;
		boneMesh->MeshData.Type = pMeshData->Type;

		// 메시의 메모리가 해제되지 않도록 참조값을 증가시킨다.
		pMeshData->pMesh->AddRef();

		// D3D 디바이스 포인터를 얻는다.
		IDirect3DDevice9 *pDevice = nullptr;//RenderMgr->GetDevice();
		pMeshData->pMesh->GetDevice(&pDevice);

		// 재질을 복사하고 텍스처를 로드한다. (스태틱 메시의 경우와 동일하다.)
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
				// 텍스처 로드
				D3DXCreateTextureFromFile(RenderMgr->GetDevice(), textureFileName, &newTexture);

			}
			boneMesh->textures.push_back(newTexture);
		}

		// 스킨 정보가 전달되었다면 이를 저장해야한다.
		if (pSkinInfo != nullptr)
		{
			// 스킨 정보를 얻는다.
			boneMesh->pSkinInfo = pSkinInfo;

			// SkinInfo가 해제되지 않도록 참조 값을 증가시킨다.
			pSkinInfo->AddRef();

			// 메시를 인덱스 블렌디드 메시로 변환하고, 본 컴비네이션 테이블을 저장해둔다
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

			// 메시를 복제해 boneMesh->MeshData->pMesh에 저장한다.
			// 이 복제된 메시는 나중에 실제로 렌더링 되는 스킨드 메시가 된다.
			pMeshData->pMesh->CloneMeshFVF(D3DXMESH_MANAGED,
				pMeshData->pMesh->GetFVF(),
				RenderMgr->GetDevice(),
				&boneMesh->MeshData.pMesh);

			// 속성 테이블을 얻는다.
			boneMesh->MeshData.pMesh->GetAttributeTable(nullptr, &boneMesh->NumAttributeGroups);
			boneMesh->attributeTables = new D3DXATTRIBUTERANGE[boneMesh->NumAttributeGroups];
			boneMesh->MeshData.pMesh->GetAttributeTable(boneMesh->attributeTables, nullptr);

			// 본의 오프셋과 합산된 변환 행렬들을 만든다.
			int NumBones = pSkinInfo->GetNumBones();
			boneMesh->boneOffsetMatrices = new D3DXMATRIX[NumBones];
			boneMesh->currentBoneMatrices = new D3DXMATRIX[NumBones];

			// 본 오프셋 행렬들을 얻는다.
			for (int i = 0; i < NumBones; i++)
				boneMesh->boneOffsetMatrices[i] = *(boneMesh->pSkinInfo->GetBoneOffsetMatrix(i));
		}

		// 새로이 생성된 boneMesh를 반환한다.
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