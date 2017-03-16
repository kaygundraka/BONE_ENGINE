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

		// �������� �޽ð� �����Ѵٸ�
		if (bone->pMeshContainer != nullptr)
		{
			BoneMesh *boneMesh = (BoneMesh*)bone->pMeshContainer;

			if (boneMesh->pSkinInfo != nullptr)
			{
				// ��ũ�� ������ ��ȯ ����� �����Ѵ�.
				// ��, ���� ���յ� ��ȯ ��İ� ������ ����� �����Ͽ� ���� ��ȯ ����� �����.
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

		// �޽ø� �����ϰ� �ִ� ��� ������ ã�´�.
		if (bone->pMeshContainer != nullptr)
		{
			BoneMesh* boneMesh = (BoneMesh*)bone->pMeshContainer;

			// ��Ų�� �޽ø� ���� ���鿡 ����, ��� �����͸� �����Ѵ�.
			if (boneMesh->pSkinInfo != nullptr)
			{
				// �� �޽ÿ� ��ũ�� ������ ���� ��´�.
				int NumBones = boneMesh->pSkinInfo->GetNumBones();

				// ���� ����ŭ�� ������ �迭�� �����.
				boneMesh->boneMatrixPtrs = new D3DXMATRIX*[NumBones];

				// �迭�� ä���.
				for (int i = 0; i < NumBones; i++)
				{
					// ��ũ�� ���� �̸����� ã�´�.
					Bone *b = (Bone*)D3DXFrameFind(rootBone, boneMesh->pSkinInfo->GetBoneName(i));

					// �׸��� �� ���� ���յ� ��ȯ ����� �����͸� �迭�� �����Ѵ�.
					if (b != nullptr)
						boneMesh->boneMatrixPtrs[i] = &b->CombinedTransformationMatrix;
					else
						boneMesh->boneMatrixPtrs[i] = nullptr;
				}
			}
		}

		// ���� ������ �������� �f�´�.
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

		// �������� �޽ð� �����Ѵٸ�
		if (bone->pMeshContainer != nullptr)
		{
			BoneMesh *boneMesh = (BoneMesh*)bone->pMeshContainer;

			if (boneMesh->pSkinInfo != nullptr)
			{
				// ��ũ�� ������ ��ȯ ����� �����Ѵ�.
				// ��, ���� ���յ� ��ȯ ��İ� ������ ����� �����Ͽ� ���� ��ȯ ����� �����.
				int numBones = boneMesh->pSkinInfo->GetNumBones();

				for (int i = 0; i < numBones; i++)
					D3DXMatrixMultiply(&boneMesh->currentBoneMatrices[i], &boneMesh->boneOffsetMatrices[i], boneMesh->boneMatrixPtrs[i]);

				// ��Ų�� �޽ø� �����Ѵ�.
				void* *src = nullptr, *dest = nullptr;

				boneMesh->OriginalMesh->LockVertexBuffer(D3DLOCK_READONLY, (VOID**)&src);
				boneMesh->MeshData.pMesh->LockVertexBuffer(0, (VOID**)&dest);

				boneMesh->pSkinInfo->UpdateSkinnedMesh(boneMesh->currentBoneMatrices, nullptr, src, dest);

				boneMesh->MeshData.pMesh->UnlockVertexBuffer();
				boneMesh->OriginalMesh->UnlockVertexBuffer();

				// �޽ø� �������Ѵ�.
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

		// ����, �ڽ� ���鿡 ���ؼ��� �������Ѵ�.
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
			// �θ� ��ȯ ��İ� ���յ� ���� ��ȯ ����� ����Ѵ�.
			D3DXMatrixMultiply(&bone->CombinedTransformationMatrix,
				&bone->TransformationMatrix,
				parentMatrix
            );
		}

		// ������ ����� ���� ���� ���� �����Ѵ�.
		if (bone->pFrameSibling)
		    UpdateMatrices((Bone*)bone->pFrameSibling, parentMatrix);
		    
		// ������ ����� �ڽ� ���� ���� �����Ѵ�.
		if (bone->pFrameFirstChild)
		{
			// ���� ���� ������ �θ� ��İ� �ٸ� ����� �����ϴ� �Ϳ� ����!
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
