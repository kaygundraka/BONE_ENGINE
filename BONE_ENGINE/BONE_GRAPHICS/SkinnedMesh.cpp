#include "Common.h"
#include "SkinnedMesh.h"
#include "RenderManager.h"
#include "SceneManager.h"

namespace BONE_GRAPHICS
{
	typedef struct _SKINNED_VERTEX {
        _SKINNED_VERTEX() {}
        _SKINNED_VERTEX(D3DXVECTOR3 pos, D3DCOLOR col) { position = pos; color = col; }
		D3DXVECTOR3 position;
		D3DCOLOR color;
		static const int FVF;
	} SKINNED_VERTEX;

	const int SKINNED_VERTEX::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;

	SkinnedMesh::SkinnedMesh()
	{
		SetTypeName("SkinnedMesh");
	}

	void SkinnedMesh::SetFileAddress(std::string address)
	{
		this->address = address;
	}

	void SkinnedMesh::LoadContent()
	{
		D3DXLoadMeshHierarchyFromX(address.c_str(), D3DXMESH_MANAGED, RenderMgr->GetDevice(),
			&boneHierarchy, nullptr, &rootBone, &animationContainer);

		UpdateMatrices((Bone*)rootBone, nullptr);

		SetupBoneMatrixPointers(nullptr);

		IsInit = true;
	}

	std::string SkinnedMesh::GetFileAddress()
	{
		return address;
	}
	
	SkinnedMesh::~SkinnedMesh() 
	{
		if (animationContainer != nullptr)
			animationContainer->Release();
	}

	void SkinnedMesh::CheckInRay(RAY* ray, Bone* bone, float& curDist)
	{
		if (bone == nullptr)
		{
			UpdateMatrices((Bone*)rootBone, nullptr);
			bone = (Bone*)rootBone;
		}

		float Temp = -1;

		// 렌더링할 메시가 존재한다면
		if (bone->pMeshContainer != nullptr)
		{
			BoneMesh *boneMesh = (BoneMesh*)bone->pMeshContainer;

			if (boneMesh->pSkinInfo != nullptr)
			{
				// 링크된 본들의 변환 행렬을 설정한다.
				// 즉, 본의 결합된 변환 행렬과 오프셋 행렬을 결합하여 최종 변환 행렬을 만든다.
				int numBones = boneMesh->pSkinInfo->GetNumBones();

				for (int i = 0; i < numBones; i++)
					D3DXMatrixMultiply(&boneMesh->currentBoneMatrices[i], &boneMesh->boneOffsetMatrices[i], boneMesh->boneMatrixPtrs[i]);

				void* *src = nullptr, *dest = nullptr;

				boneMesh->OriginalMesh->LockVertexBuffer(D3DLOCK_READONLY, (VOID**)&src);
				boneMesh->MeshData.pMesh->LockVertexBuffer(0, (VOID**)&dest);

				boneMesh->pSkinInfo->UpdateSkinnedMesh(boneMesh->currentBoneMatrices, nullptr, src, dest);

				boneMesh->MeshData.pMesh->UnlockVertexBuffer();
				boneMesh->OriginalMesh->UnlockVertexBuffer();

				for (UINT i = 0; i < boneMesh->NumAttributeGroups; i++)
				{
					Matrix matIden;
					D3DXMatrixIdentity(&matIden);
					if (RenderMgr->CheckRayInMesh(matIden, boneMesh->MeshData.pMesh, &Temp))
						if (Temp <= curDist)
							curDist = Temp;
				}
			}
		}

        if (bone->pFrameSibling != nullptr)
			CheckInRay(ray, (Bone*)bone->pFrameSibling, curDist);

		if (bone->pFrameFirstChild != nullptr)
			CheckInRay(ray, (Bone*)bone->pFrameFirstChild, curDist);
	}

	void SkinnedMesh::SetupBoneMatrixPointers(Bone* bone)
	{
		if (bone == nullptr)
			bone = (Bone*)rootBone;

		// 메시를 포함하고 있는 모든 본들을 찾는다.
		if (bone->pMeshContainer != nullptr)
		{
			BoneMesh* boneMesh = (BoneMesh*)bone->pMeshContainer;

			// 스킨드 메시를 가진 본들에 대해, 행렬 포인터를 설정한다.
			if (boneMesh->pSkinInfo != nullptr)
			{
				// 이 메시와 링크된 본들의 수를 얻는다.
				int NumBones = boneMesh->pSkinInfo->GetNumBones();

				// 본의 수만큼의 포인터 배열을 만든다.
				boneMesh->boneMatrixPtrs = new D3DXMATRIX*[NumBones];

				// 배열을 채운다.
				for (int i = 0; i < NumBones; i++)
				{
					// 링크된 본을 이름으로 찾는다.
					Bone *b = (Bone*)D3DXFrameFind(rootBone, boneMesh->pSkinInfo->GetBoneName(i));

					// 그리고 그 본의 결합된 변환 행렬의 포인터를 배열에 저장한다.
					if (b != nullptr)
						boneMesh->boneMatrixPtrs[i] = &b->CombinedTransformationMatrix;
					else
						boneMesh->boneMatrixPtrs[i] = nullptr;
				}
			}
		}

		// 계층 구조의 나머지를 훓는다.
		if (bone->pFrameSibling != nullptr)
			SetupBoneMatrixPointers((Bone*)bone->pFrameSibling);

		if (bone->pFrameFirstChild != nullptr)
			SetupBoneMatrixPointers((Bone*)bone->pFrameFirstChild);
	}

	void SkinnedMesh::PlayAnimation(std::string animationName, bool isPlay, bool rotateLoop)
	{
		if (this->animationName != animationName)
		{
			animationSet[this->animationName].IsPlay = false;
			animationSet[this->animationName].IsPlay = true;

			animationContainer->KeyTrackSpeed(0, 1.0f, 0, 0.1f, D3DXTRANSITION_LINEAR);
			animationContainer->KeyTrackWeight(0, 1.0f, 0, 0.1f, D3DXTRANSITION_LINEAR);
			animationContainer->SetTrackAnimationSet(0, animationSet[animationName].AnimationSet);
			animationContainer->SetTrackEnable(animationSet[this->animationName].Vertex_Index, false);
			animationContainer->SetTrackEnable(animationSet[animationName].Vertex_Index, true);
            this->animationName = animationName;

			this->IsPlay = isPlay;
			this->IsRotateLoop = rotateLoop;
		}
	}

	void SkinnedMesh::UpdateAnimation()
	{
		animationContainer->AdvanceTime(
            SceneMgr->GetTimeDelta() * animationSet[animationName].AnimationSpeed, 
            nullptr
        );
	}

	void SkinnedMesh::SetAnimation(std::string name, int index, float animationSpeed)
	{
		animationName = name;

		animationSet[animationName].AnimationSet = nullptr;
		animationSet[animationName].AnimationSpeed = animationSpeed;
		animationSet[animationName].Vertex_Index = index;
		animationSet[animationName].IsPlay = false;

		animationContainer->GetAnimationSet(animationSet[animationName].Vertex_Index, &animationSet[animationName].AnimationSet);
	}

	void SkinnedMesh::SetAnimation(std::string name, float animationSpeed)
	{
		animationName = name;

		animationSet[animationName].AnimationSet = nullptr;
		animationSet[animationName].AnimationSpeed = animationSpeed;
		animationSet[animationName].IsPlay = false;

		animationContainer->GetAnimationSetByName(animationName.c_str(), &animationSet[animationName].AnimationSet);
		animationSet[animationName].AnimationSet->GetAnimationIndexByName(animationName.c_str(), &animationSet[animationName].Vertex_Index);
	}

	void SkinnedMesh::SoftwareRendering(IShader* shaderOpt, GameObject* object, Bone* bone)
	{
		if (bone == nullptr)
			bone = (Bone*)rootBone;

		// 렌더링할 메시가 존재한다면
		if (bone->pMeshContainer != nullptr)
		{
			BoneMesh *boneMesh = (BoneMesh*)bone->pMeshContainer;

			if (boneMesh->pSkinInfo != nullptr)
			{
				// 링크된 본들의 변환 행렬을 설정한다.
				// 즉, 본의 결합된 변환 행렬과 오프셋 행렬을 결합하여 최종 변환 행렬을 만든다.
				int numBones = boneMesh->pSkinInfo->GetNumBones();

				for (int i = 0; i < numBones; i++)
					D3DXMatrixMultiply(&boneMesh->currentBoneMatrices[i], &boneMesh->boneOffsetMatrices[i], boneMesh->boneMatrixPtrs[i]);

				// 스킨드 메시를 갱신한다.
				void* *src = nullptr, *dest = nullptr;

				boneMesh->OriginalMesh->LockVertexBuffer(D3DLOCK_READONLY, (VOID**)&src);
				boneMesh->MeshData.pMesh->LockVertexBuffer(0, (VOID**)&dest);

				boneMesh->pSkinInfo->UpdateSkinnedMesh(boneMesh->currentBoneMatrices, nullptr, src, dest);

				boneMesh->MeshData.pMesh->UnlockVertexBuffer();
				boneMesh->OriginalMesh->UnlockVertexBuffer();

				// 메시를 렌더링한다.
				if (shaderOpt == nullptr)
				{
					for (UINT i = 0; i < boneMesh->NumAttributeGroups; i++)
					{
						int mtrl = boneMesh->attributeTables[i].AttribId;

						RenderMgr->GetDevice()->SetMaterial(&(boneMesh->materials[mtrl]));

						RenderMgr->GetDevice()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
						RenderMgr->GetDevice()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);

						RenderMgr->GetDevice()->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
						RenderMgr->GetDevice()->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
						RenderMgr->GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSU, 0);
						RenderMgr->GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSV, 0);

						RenderMgr->GetDevice()->SetTexture(0, boneMesh->textures[mtrl]);
						boneMesh->MeshData.pMesh->DrawSubset(mtrl);
					}
				}
				else
				{
					for (UINT i = 0; i < boneMesh->NumAttributeGroups; i++)
					{
						int mtrl = boneMesh->attributeTables[i].AttribId;

						shaderOpt->Render(i, object);
						
						boneMesh->MeshData.pMesh->DrawSubset(mtrl);
					}
				}
			}
		}

		// 형제, 자식 본들에 대해서도 렌더링한다.
		if (bone->pFrameSibling != nullptr)
			SoftwareRendering(shaderOpt, object, (Bone*)bone->pFrameSibling);

		if (bone->pFrameFirstChild != nullptr)
			SoftwareRendering(shaderOpt, object, (Bone*)bone->pFrameFirstChild);
	}

	void SkinnedMesh::UpdateMatrices(Bone* bone, D3DXMATRIX* parentMatrix)
	{
		if (parentMatrix == nullptr)
			bone->CombinedTransformationMatrix = bone->TransformationMatrix;
		else
		{
			// 부모 변환 행렬과 복합된 월드 변환 행렬을 계산한다.
			D3DXMatrixMultiply(&bone->CombinedTransformationMatrix,
				&bone->TransformationMatrix,
				parentMatrix
            );
		}

		// 동일한 계산을 형제 본에 대해 수행한다.
		if (bone->pFrameSibling)
		    UpdateMatrices((Bone*)bone->pFrameSibling, parentMatrix);
		    
		// 동일한 계산을 자식 본에 대해 수행한다.
		if (bone->pFrameFirstChild)
		{
			// 형제 본에 전달한 부모 행렬과 다른 행렬을 전달하는 것에 주의!
			UpdateMatrices(
                (Bone*)bone->pFrameFirstChild,
				&bone->CombinedTransformationMatrix
            );
		}
	}

	void SkinnedMesh::Render(IShader* shaderOpt, GameObject* object)
	{
		D3DXMATRIX A;
		D3DXMatrixIdentity(&A);
		RenderMgr->GetDevice()->SetTransform(D3DTS_WORLD, &A);

		UpdateMatrices((Bone*)rootBone, nullptr);

		SoftwareRendering(shaderOpt, object, nullptr);
	}

	LPD3DXMESH SkinnedMesh::GetMesh()
	{
		return rootBone->pMeshContainer->MeshData.pMesh;
	}

    map<std::string, KEYFRAMERATE> SkinnedMesh::GetAnmimationSet()
    {
        return animationSet;
    }

}
