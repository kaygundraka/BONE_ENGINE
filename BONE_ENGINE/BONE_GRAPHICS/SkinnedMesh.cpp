#include "Common.h"
#include "SkinnedMesh.h"
#include "RenderManager.h"

namespace BONE_GRAPHICS
{
	struct etuSkinnedVertex {
		etuSkinnedVertex() {}
		etuSkinnedVertex(D3DXVECTOR3 pos, D3DCOLOR col) { position = pos; color = col; }
		D3DXVECTOR3 position;
		D3DCOLOR color;
		static const DWORD FVF;
	};

	const DWORD etuSkinnedVertex::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;

	SkinnedMesh::SkinnedMesh()
	{
		CThreadSync sync;

		SetTypeName("SkinnedMesh");
	}

	void SkinnedMesh::SetFileAddress(string _address)
	{
		CThreadSync sync;

		address = _address;
	}

	void SkinnedMesh::LoadContent()
	{
		CThreadSync sync;

		// .x���Ϸκ��� ���� ������ �ε��Ѵ�.
		D3DXLoadMeshHierarchyFromX(address.c_str(), D3DXMESH_MANAGED, RenderMgr->GetDevice(),
			&boneHierarchy, NULL, &rootBone, &animationContainer);

		// ��� ���� ��ȯ ��ĵ��� ������Ʈ �Ѵ�.
		UpdateMatrices((Bone*)rootBone, NULL);

		SetupBoneMatrixPointers(NULL);

		IsInit = true;
	}

	string SkinnedMesh::GetFileAddress()
	{
		CThreadSync sync;

		return address;
	}
	
	SkinnedMesh::~SkinnedMesh() 
	{
		CThreadSync sync;

		if (animationContainer != NULL)
			animationContainer->Release();
	}

	void SkinnedMesh::CheckInRay(Ray* _ray, Bone* _bone, float& _curDist)
	{
		CThreadSync sync;

		if (_bone == NULL)
		{
			UpdateMatrices((Bone*)rootBone, NULL);
			_bone = (Bone*)rootBone;
		}

		float Temp = -1;

		// �������� �޽ð� �����Ѵٸ�
		if (_bone->pMeshContainer != NULL)
		{
			BoneMesh *boneMesh = (BoneMesh*)_bone->pMeshContainer;

			if (boneMesh->pSkinInfo != NULL)
			{
				// ��ũ�� ������ ��ȯ ����� �����Ѵ�.
				// ��, ���� ���յ� ��ȯ ��İ� ������ ����� �����Ͽ� ���� ��ȯ ����� �����.
				int numBones = boneMesh->pSkinInfo->GetNumBones();

				for (int i = 0; i < numBones; i++)
					D3DXMatrixMultiply(&boneMesh->currentBoneMatrices[i], &boneMesh->boneOffsetMatrices[i], boneMesh->boneMatrixPtrs[i]);

				// ��Ų�� �޽ø� �����Ѵ�.
				void* *src = NULL, *dest = NULL;

				boneMesh->OriginalMesh->LockVertexBuffer(D3DLOCK_READONLY, (VOID**)&src);
				boneMesh->MeshData.pMesh->LockVertexBuffer(0, (VOID**)&dest);

				boneMesh->pSkinInfo->UpdateSkinnedMesh(boneMesh->currentBoneMatrices, NULL, src, dest);

				boneMesh->MeshData.pMesh->UnlockVertexBuffer();
				boneMesh->OriginalMesh->UnlockVertexBuffer();

				for (UINT i = 0; i < boneMesh->NumAttributeGroups; i++)
				{
					D3DXMATRIX matIden;
					D3DXMatrixIdentity(&matIden);
					if (RenderMgr->CheckRayInMesh(_ray, matIden, boneMesh->MeshData.pMesh, &Temp))
						if (Temp <= _curDist)
							_curDist = Temp;
				}
			}
		}

		// ����, �ڽ� ���鿡 ���ؼ��� �������Ѵ�.
		if (_bone->pFrameSibling != NULL)
			CheckInRay(_ray, (Bone*)_bone->pFrameSibling, _curDist);

		if (_bone->pFrameFirstChild != NULL)
			CheckInRay(_ray, (Bone*)_bone->pFrameFirstChild, _curDist);
	}

	void SkinnedMesh::SetupBoneMatrixPointers(Bone* bone)
	{
		CThreadSync sync;

		if (bone == NULL)
			bone = (Bone*)rootBone;

		// �޽ø� �����ϰ� �ִ� ��� ������ ã�´�.
		if (bone->pMeshContainer != NULL)
		{
			BoneMesh* boneMesh = (BoneMesh*)bone->pMeshContainer;

			// ��Ų�� �޽ø� ���� ���鿡 ����, ��� �����͸� �����Ѵ�.
			if (boneMesh->pSkinInfo != NULL)
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
					if (b != NULL)
					{
						boneMesh->boneMatrixPtrs[i] = &b->CombinedTransformationMatrix;
					}
					else
					{
						boneMesh->boneMatrixPtrs[i] = NULL;
					}
				}
			}
		}

		// ���� ������ �������� �f�´�.
		if (bone->pFrameSibling != NULL)
			SetupBoneMatrixPointers((Bone*)bone->pFrameSibling);

		if (bone->pFrameFirstChild != NULL)
			SetupBoneMatrixPointers((Bone*)bone->pFrameFirstChild);
	}

	void SkinnedMesh::PlayAnimation(string _animationName, bool _IsPlay, bool _rotateLoop)
	{
		CThreadSync sync;

		if (animationName != _animationName)
		{
			animationSet[animationName].IsPlay = false;
			animationSet[animationName].IsPlay = true;

			animationContainer->KeyTrackSpeed(0, 1.0f, 0, 0.1f, D3DXTRANSITION_LINEAR);
			animationContainer->KeyTrackWeight(0, 1.0f, 0, 0.1f, D3DXTRANSITION_LINEAR);
			animationContainer->SetTrackAnimationSet(0, animationSet[_animationName].AnimationSet);
			animationContainer->SetTrackEnable(animationSet[animationName].Vertex_Index, false);
			animationContainer->SetTrackEnable(animationSet[_animationName].Vertex_Index, true);
			animationName = animationName;

			IsPlay = _IsPlay;
			IsRotateLoop = _rotateLoop;
		}
	}

	void SkinnedMesh::UpdateAnimation(float _timeDelta)
	{
		CThreadSync sync;

		animationContainer->AdvanceTime(_timeDelta * animationSet[animationName].AnimationSpeed, NULL);
	}

	void SkinnedMesh::SetAnimation(string _name, int _index, float _animationSpeed)
	{
		CThreadSync sync;

		animationName = _name;

		animationSet[animationName].AnimationSet = NULL;
		animationSet[animationName].AnimationSpeed = _animationSpeed;
		animationSet[animationName].Vertex_Index = _index;
		animationSet[animationName].IsPlay = false;

		animationContainer->GetAnimationSet(animationSet[animationName].Vertex_Index, &animationSet[animationName].AnimationSet);
	}

	void SkinnedMesh::SetAnimation(string _name, float _animationSpeed)
	{
		CThreadSync sync;

		animationName = _name;

		animationSet[animationName].AnimationSet = NULL;
		animationSet[animationName].AnimationSpeed = _animationSpeed;
		animationSet[animationName].IsPlay = false;

		animationContainer->GetAnimationSetByName(animationName.c_str(), &animationSet[animationName].AnimationSet);
		animationSet[animationName].AnimationSet->GetAnimationIndexByName(animationName.c_str(), &animationSet[animationName].Vertex_Index);
	}

	void SkinnedMesh::SoftwareRendering(IShader* _shaderOption, GameObject* _object, Bone* _bone)
	{
		CThreadSync sync;

		if (_bone == NULL)
			_bone = (Bone*)rootBone;

		// �������� �޽ð� �����Ѵٸ�
		if (_bone->pMeshContainer != NULL)
		{
			BoneMesh *boneMesh = (BoneMesh*)_bone->pMeshContainer;

			if (boneMesh->pSkinInfo != NULL)
			{
				// ��ũ�� ������ ��ȯ ����� �����Ѵ�.
				// ��, ���� ���յ� ��ȯ ��İ� ������ ����� �����Ͽ� ���� ��ȯ ����� �����.
				int numBones = boneMesh->pSkinInfo->GetNumBones();

				for (int i = 0; i < numBones; i++)
					D3DXMatrixMultiply(&boneMesh->currentBoneMatrices[i], &boneMesh->boneOffsetMatrices[i], boneMesh->boneMatrixPtrs[i]);

				// ��Ų�� �޽ø� �����Ѵ�.
				void* *src = NULL, *dest = NULL;

				boneMesh->OriginalMesh->LockVertexBuffer(D3DLOCK_READONLY, (VOID**)&src);
				boneMesh->MeshData.pMesh->LockVertexBuffer(0, (VOID**)&dest);

				boneMesh->pSkinInfo->UpdateSkinnedMesh(boneMesh->currentBoneMatrices, NULL, src, dest);

				boneMesh->MeshData.pMesh->UnlockVertexBuffer();
				boneMesh->OriginalMesh->UnlockVertexBuffer();

				// �޽ø� �������Ѵ�.
				if (_shaderOption == NULL)
				{
					for (UINT i = 0; i < boneMesh->NumAttributeGroups; i++)
					{
						int mtrl = boneMesh->attributeTables[i].AttribId;

						RenderMgr->GetDevice()->SetMaterial(&(boneMesh->materials[mtrl]));

						RenderMgr->GetDevice()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
						RenderMgr->GetDevice()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);

						RenderMgr->GetDevice()->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
						RenderMgr->GetDevice()->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
						RenderMgr->GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSU, NULL);
						RenderMgr->GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSV, NULL);

						RenderMgr->GetDevice()->SetTexture(0, boneMesh->textures[mtrl]);
						boneMesh->MeshData.pMesh->DrawSubset(mtrl);
					}
				}
				else
				{
					for (UINT i = 0; i < boneMesh->NumAttributeGroups; i++)
					{
						int mtrl = boneMesh->attributeTables[i].AttribId;

						_shaderOption->Render(i, _object);
						
						boneMesh->MeshData.pMesh->DrawSubset(mtrl);
					}
				}
			}
		}

		// ����, �ڽ� ���鿡 ���ؼ��� �������Ѵ�.
		if (_bone->pFrameSibling != NULL)
			SoftwareRendering(_shaderOption, _object, (Bone*)_bone->pFrameSibling);

		if (_bone->pFrameFirstChild != NULL)
			SoftwareRendering(_shaderOption, _object, (Bone*)_bone->pFrameFirstChild);
	}

	void SkinnedMesh::UpdateMatrices(Bone* _bone, D3DXMATRIX* _parentMatrix)
	{
		CThreadSync sync;

		if (_parentMatrix == NULL)
			_bone->CombinedTransformationMatrix = _bone->TransformationMatrix;
		else
		{
			// �θ� ��ȯ ��İ� ���յ� ���� ��ȯ ����� ����Ѵ�.
			D3DXMatrixMultiply(&_bone->CombinedTransformationMatrix,
				&_bone->TransformationMatrix,
				_parentMatrix);
		}

		// ������ ����� ���� ���� ���� �����Ѵ�.
		if (_bone->pFrameSibling)
		{
			UpdateMatrices((Bone*)_bone->pFrameSibling, _parentMatrix);
		}

		// ������ ����� �ڽ� ���� ���� �����Ѵ�.
		if (_bone->pFrameFirstChild)
		{
			// ���� ���� ������ �θ� ��İ� �ٸ� ����� �����ϴ� �Ϳ� ����!
			UpdateMatrices((Bone*)_bone->pFrameFirstChild,
				&_bone->CombinedTransformationMatrix);
		}
	}

	void SkinnedMesh::Render(IShader* _shaderOption, GameObject* _object, float _timeDelta)
	{
		CThreadSync sync;

		D3DXMATRIX A;
		D3DXMatrixIdentity(&A);
		RenderMgr->GetDevice()->SetTransform(D3DTS_WORLD, &A);

		// ��� ���� ��ȯ ��ĵ��� ������Ʈ �Ѵ�.
		UpdateMatrices((Bone*)rootBone, NULL);

		SoftwareRendering(_shaderOption, _object, NULL);
	}

	LPD3DXMESH SkinnedMesh::GetMesh()
	{
		CThreadSync sync;

		return rootBone->pMeshContainer->MeshData.pMesh;
	}
}
