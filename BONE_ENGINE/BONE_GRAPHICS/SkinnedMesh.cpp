#include "Common.h"
#include "SkinnedMesh.h"
#include "RenderManager.h"
#include "SceneManager.h"
#include "ResourceManager.h"

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

    void SkinnedMesh::SetFile(std::string fileName)
    {
        this->fileName = fileName;
    }

    void SkinnedMesh::LoadContent()
    {
        std::string FullPath;

        if (!ResourceMgr->ExistFile(fileName, &FullPath))
            return;

        D3DXLoadMeshHierarchyFromX(FullPath.c_str(), D3DXMESH_MANAGED, RenderMgr->GetDevice(),
            &boneHierarchy, nullptr, &rootBone, &animationContainer);

        UpdateMatrices((Bone*)rootBone, nullptr);

        SetupBoneMatrixPointers(nullptr);

        IsInit = true;

        GetAnimationSets();
    }

    std::string SkinnedMesh::GetFile()
    {
        return fileName;
    }

    SkinnedMesh::~SkinnedMesh()
    {
        if (animationContainer != nullptr)
            animationContainer->Release();
    }

    bool SkinnedMesh::CheckInRay(RAY* ray, Bone* bone, float& curDist)
    {
        if (bone == nullptr)
            bone = (Bone*)rootBone;

        static bool exist = false;

        float Temp = -1;

        if (bone->pMeshContainer != nullptr)
        {
            BoneMesh *boneMesh = (BoneMesh*)bone->pMeshContainer;

            if (boneMesh->pSkinInfo != nullptr)
            {
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
                        {
                            exist = true;
                            curDist = Temp;
                        }
                }
            }
        }

        if (bone->pFrameSibling != nullptr)
            CheckInRay(ray, (Bone*)bone->pFrameSibling, curDist);

        if (bone->pFrameFirstChild != nullptr)
            CheckInRay(ray, (Bone*)bone->pFrameFirstChild, curDist);

        return exist;
    }

    void SkinnedMesh::SetupBoneMatrixPointers(Bone* bone)
    {
        if (bone == nullptr)
            bone = (Bone*)rootBone;

        if (bone->pMeshContainer != nullptr)
        {
            BoneMesh* boneMesh = (BoneMesh*)bone->pMeshContainer;

            if (boneMesh->pSkinInfo != nullptr)
            {
                int NumBones = boneMesh->pSkinInfo->GetNumBones();

                boneMesh->boneMatrixPtrs = new D3DXMATRIX*[NumBones];

                for (int i = 0; i < NumBones; i++)
                {
                    Bone *b = (Bone*)D3DXFrameFind(rootBone, boneMesh->pSkinInfo->GetBoneName(i));

                    if (b != nullptr)
                        boneMesh->boneMatrixPtrs[i] = &b->CombinedTransformationMatrix;
                    else
                        boneMesh->boneMatrixPtrs[i] = nullptr;
                }
            }
        }

        if (bone->pFrameSibling != nullptr)
            SetupBoneMatrixPointers((Bone*)bone->pFrameSibling);

        if (bone->pFrameFirstChild != nullptr)
            SetupBoneMatrixPointers((Bone*)bone->pFrameFirstChild);
    }

    void SkinnedMesh::UpdateAnimation()
    {
        animationContainer->AdvanceTime(
            SceneMgr->GetTimeDelta(),
            nullptr
        );
    }
    
    void SkinnedMesh::GetAnimationSets() {
        ID3DXAnimationSet* pAnim = NULL;

        for (int i = 0; i<(int)animationContainer->GetMaxNumAnimationSets(); i++)
        {
            pAnim = NULL;
            animationContainer->GetAnimationSet(i, &pAnim);

            //If we found an animation set, add it to the map
            if (pAnim != NULL)
            {
                string name = pAnim->GetName();
                animationSets[name] = i;//Creates an entry
                pAnim->Release();
            }
        }

        SetAnimation(animationSets.begin()->first);
    }

    void SkinnedMesh::SetAnimation(std::string name) {
        ID3DXAnimationSet* pAnim = NULL;
        //Get the animation set from the name.
        animationContainer->GetAnimationSet(animationSets[name], &pAnim);

        if (pAnim != NULL)
        {
            //Set the current animation set
            animationContainer->SetTrackAnimationSet(0, pAnim);
            pAnim->Release();
        }
    }

    void SkinnedMesh::SoftwareRendering(IShader* shaderOpt, GameObject* object, Bone* bone)
    {
        if (bone == nullptr)
            bone = (Bone*)rootBone;
        
        if (bone->pMeshContainer != nullptr)
        {
            BoneMesh *boneMesh = (BoneMesh*)bone->pMeshContainer;

            if (boneMesh->pSkinInfo != nullptr)
            {
                int numBones = boneMesh->pSkinInfo->GetNumBones();

                for (int i = 0; i < numBones; i++)
                    D3DXMatrixMultiply(&boneMesh->currentBoneMatrices[i], &boneMesh->boneOffsetMatrices[i], boneMesh->boneMatrixPtrs[i]);

                void* *src = nullptr, *dest = nullptr;

                boneMesh->OriginalMesh->LockVertexBuffer(D3DLOCK_READONLY, (VOID**)&src);
                boneMesh->MeshData.pMesh->LockVertexBuffer(0, (VOID**)&dest);

                boneMesh->pSkinInfo->UpdateSkinnedMesh(boneMesh->currentBoneMatrices, nullptr, src, dest);

                boneMesh->MeshData.pMesh->UnlockVertexBuffer();
                boneMesh->OriginalMesh->UnlockVertexBuffer();

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

                        if (boneMesh->textures.size() != 0)
                            RenderMgr->GetDevice()->SetTexture(0, boneMesh->textures[mtrl]);
                        boneMesh->MeshData.pMesh->DrawSubset(mtrl);
                    }
                }
                else
                {
                    for (UINT i = 0; i < boneMesh->NumAttributeGroups; i++)
                    {
                        int mtrl = boneMesh->attributeTables[i].AttribId;

                        shaderOpt->Render(i, boneMesh->textures, object);
                        boneMesh->MeshData.pMesh->DrawSubset(mtrl);
                    }
                }
            }
        }

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
            D3DXMatrixMultiply(&bone->CombinedTransformationMatrix,
                &bone->TransformationMatrix,
                parentMatrix
            );
        }

        /*D3DXMatrixMultiply(&bone->CombinedTransformationMatrix,
            &bone->TransformationMatrix,
            parentMatrix
        );*/
        
        if (bone->pFrameSibling)
            UpdateMatrices((Bone*)bone->pFrameSibling, parentMatrix);

        if (bone->pFrameFirstChild)
        {
            UpdateMatrices(
                (Bone*)bone->pFrameFirstChild,
                &bone->CombinedTransformationMatrix
            );
        }
    }

    void SkinnedMesh::Render(IShader* shaderOpt, GameObject* object)
    {
        UpdateMatrices((Bone*)rootBone, nullptr);//&((Transform3D*)object->transform3D)->GetTransform());

        SoftwareRendering(shaderOpt, object, nullptr);
    }

    Matrix* SkinnedMesh::GetBoneMatrix(std::string name, Bone* bone, Matrix* mat)
    {
        static Matrix* result = nullptr;

        if (bone == nullptr)
            bone = (Bone*)rootBone;
        
        if (mat == nullptr)
            bone->CombinedTransformationMatrix = bone->TransformationMatrix;
        else
        {
            D3DXMatrixMultiply(&bone->CombinedTransformationMatrix,
                &bone->TransformationMatrix,
                mat
            );
        }

        if (name == bone->Name)
            return result = &(bone->CombinedTransformationMatrix);

        if (bone->pFrameSibling != nullptr)
            GetBoneMatrix(name, (Bone*)bone->pFrameSibling, &bone->CombinedTransformationMatrix);

        if (bone->pFrameFirstChild != nullptr)
            GetBoneMatrix(name, (Bone*)bone->pFrameFirstChild, &bone->CombinedTransformationMatrix);

        return result;
    }

    map<std::string, int> SkinnedMesh::GetAnmimationSet()
    {
        return animationSets;
    }
}