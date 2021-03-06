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
        animationLoop = true;
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

        LPD3DXANIMATIONSET temp;
        animationContainer->GetAnimationSet(0, &temp);
        curAnimation = temp->GetName();

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
        if (GetAnimationRate() < 0.99f || animationLoop)
        {
            animationContainer->AdvanceTime(
                SceneMgr->GetTimeDelta(),
                nullptr
            );
        }
        
        if (GetAnimationRate() >= 0.99f && !animationLoop) {

            ID3DXAnimationSet* pAnim = NULL;
            animationContainer->GetAnimationSet(animationSets[curAnimation], &pAnim);

            animationContainer->SetTrackPosition(0, pAnim->GetPeriod());
        }
    }

    void SkinnedMesh::SetAnimationLoop(bool loop)
    {
        animationLoop = loop;
    }
    
    bool SkinnedMesh::GetAnimationLoop()
    {
        return animationLoop;
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

    double SkinnedMesh::GetAnimationRate()
    {
        ID3DXAnimationSet* pAnim = NULL;
        //Get the animation set from the name.
        animationContainer->GetAnimationSet(animationSets[curAnimation], &pAnim);
     
        D3DXTRACK_DESC Track;
        animationContainer->GetTrackDesc(0, &Track);

        double result = Track.Position / pAnim->GetPeriod();

        if (result >= 0.99f && animationLoop == false)
        {
            result = 1.0f;
            animationContainer->SetTrackPosition(0, pAnim->GetPeriod());
        }

        return result;
    }

    void SkinnedMesh::ResetTime()
    {
        animationContainer->ResetTime();
    }

    void SkinnedMesh::SetAnimation(std::string name) {
        if (name == curAnimation)
            return;

        ID3DXAnimationSet* pAnim = NULL;
        animationContainer->GetAnimationSet(animationSets[name], &pAnim);

        if (pAnim != NULL)
        {
            animationContainer->UnkeyAllTrackEvents(0);
            animationContainer->ResetTime();
            animationContainer->SetTrackAnimationSet(0, pAnim);
            animationContainer->SetTrackEnable(0, TRUE);
            animationContainer->SetTrackPosition(0, 0.0);
            curAnimation = pAnim->GetName();
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

                        if (boneMesh->diffuseTextures.size() != 0)
                            RenderMgr->GetDevice()->SetTexture(0, boneMesh->diffuseTextures[mtrl]);
                        boneMesh->MeshData.pMesh->DrawSubset(mtrl);
                    }
                }
                else
                {
                    for (UINT i = 0; i < boneMesh->NumAttributeGroups; i++)
                    {
                        int mtrl = boneMesh->attributeTables[i].AttribId;

                        shaderOpt->Render(i, boneMesh->diffuseTextures, object);
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
        UpdateMatrices((Bone*)rootBone, nullptr);

        SoftwareRendering(shaderOpt, object, nullptr);
    }

    void SkinnedMesh::Render(ID3DXEffect* effect, GameObject* object)
    {
        UpdateMatrices((Bone*)rootBone, nullptr);

        SoftwareRendering(effect, object, nullptr);
    }

    void SkinnedMesh::SoftwareRendering(ID3DXEffect* effect, GameObject* object, Bone* bone)
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

                Matrix matWorld, matWorldIT;
                matWorld = ((Transform3D*)object->transform3D)->GetTransform();
                D3DXMatrixInverse(&matWorldIT, NULL, &matWorld);
                D3DXMatrixTranspose(&matWorldIT, &matWorldIT);
                
                effect->SetMatrix("matWorld", &matWorld);
                effect->SetMatrix("matWorldIT", &matWorldIT);
                
                for (UINT i = 0; i < boneMesh->NumAttributeGroups; i++)
                {
                    int mtrl = boneMesh->attributeTables[i].AttribId;

                    if (boneMesh->normalTextures.size() > i)
                    {
                        effect->SetTexture("normalMapTexture", boneMesh->normalTextures[i]);
                        effect->SetTexture("tColorMap", boneMesh->diffuseTextures[i]);
                    }

                    effect->CommitChanges();

                    boneMesh->MeshData.pMesh->DrawSubset(mtrl);
                }
            }
        }

        if (bone->pFrameSibling != nullptr)
            SoftwareRendering(effect, object, (Bone*)bone->pFrameSibling);

        if (bone->pFrameFirstChild != nullptr)
            SoftwareRendering(effect, object, (Bone*)bone->pFrameFirstChild);
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
        {
            return result = &(bone->CombinedTransformationMatrix);
        }

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

    std::string SkinnedMesh::GetCurrentAnimation()
    {
        return curAnimation;
    }
}