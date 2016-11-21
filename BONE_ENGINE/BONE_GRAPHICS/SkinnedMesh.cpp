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

		// .x파일로부터 계층 구조를 로드한다.
		D3DXLoadMeshHierarchyFromX(address.c_str(), D3DXMESH_MANAGED, RenderMgr->GetDevice(),
			&boneHierarchy, NULL, &rootBone, &animationContainer);

		// 모든 본의 변환 행렬들을 업데이트 한다.
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

		// 렌더링할 메시가 존재한다면
		if (_bone->pMeshContainer != NULL)
		{
			BoneMesh *boneMesh = (BoneMesh*)_bone->pMeshContainer;

			if (boneMesh->pSkinInfo != NULL)
			{
				// 링크된 본들의 변환 행렬을 설정한다.
				// 즉, 본의 결합된 변환 행렬과 오프셋 행렬을 결합하여 최종 변환 행렬을 만든다.
				int numBones = boneMesh->pSkinInfo->GetNumBones();

				for (int i = 0; i < numBones; i++)
					D3DXMatrixMultiply(&boneMesh->currentBoneMatrices[i], &boneMesh->boneOffsetMatrices[i], boneMesh->boneMatrixPtrs[i]);

				// 스킨드 메시를 갱신한다.
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

		// 형제, 자식 본들에 대해서도 렌더링한다.
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

		// 메시를 포함하고 있는 모든 본들을 찾는다.
		if (bone->pMeshContainer != NULL)
		{
			BoneMesh* boneMesh = (BoneMesh*)bone->pMeshContainer;

			// 스킨드 메시를 가진 본들에 대해, 행렬 포인터를 설정한다.
			if (boneMesh->pSkinInfo != NULL)
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

		// 계층 구조의 나머지를 훓는다.
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

		// 렌더링할 메시가 존재한다면
		if (_bone->pMeshContainer != NULL)
		{
			BoneMesh *boneMesh = (BoneMesh*)_bone->pMeshContainer;

			if (boneMesh->pSkinInfo != NULL)
			{
				// 링크된 본들의 변환 행렬을 설정한다.
				// 즉, 본의 결합된 변환 행렬과 오프셋 행렬을 결합하여 최종 변환 행렬을 만든다.
				int numBones = boneMesh->pSkinInfo->GetNumBones();

				for (int i = 0; i < numBones; i++)
					D3DXMatrixMultiply(&boneMesh->currentBoneMatrices[i], &boneMesh->boneOffsetMatrices[i], boneMesh->boneMatrixPtrs[i]);

				// 스킨드 메시를 갱신한다.
				void* *src = NULL, *dest = NULL;

				boneMesh->OriginalMesh->LockVertexBuffer(D3DLOCK_READONLY, (VOID**)&src);
				boneMesh->MeshData.pMesh->LockVertexBuffer(0, (VOID**)&dest);

				boneMesh->pSkinInfo->UpdateSkinnedMesh(boneMesh->currentBoneMatrices, NULL, src, dest);

				boneMesh->MeshData.pMesh->UnlockVertexBuffer();
				boneMesh->OriginalMesh->UnlockVertexBuffer();

				// 메시를 렌더링한다.
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

		// 형제, 자식 본들에 대해서도 렌더링한다.
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
			// 부모 변환 행렬과 복합된 월드 변환 행렬을 계산한다.
			D3DXMatrixMultiply(&_bone->CombinedTransformationMatrix,
				&_bone->TransformationMatrix,
				_parentMatrix);
		}

		// 동일한 계산을 형제 본에 대해 수행한다.
		if (_bone->pFrameSibling)
		{
			UpdateMatrices((Bone*)_bone->pFrameSibling, _parentMatrix);
		}

		// 동일한 계산을 자식 본에 대해 수행한다.
		if (_bone->pFrameFirstChild)
		{
			// 형제 본에 전달한 부모 행렬과 다른 행렬을 전달하는 것에 주의!
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

		// 모든 본의 변환 행렬들을 업데이트 한다.
		UpdateMatrices((Bone*)rootBone, NULL);

		SoftwareRendering(_shaderOption, _object, NULL);
	}

	LPD3DXMESH SkinnedMesh::GetMesh()
	{
		CThreadSync sync;

		return rootBone->pMeshContainer->MeshData.pMesh;
	}
}
