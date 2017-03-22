#include "Common.h"
#include "SkinnedMesh.h"
#include "RenderManager.h"
#include "SceneManager.h"
#include "Transform3D.h"
#include "ResourceManager.h"
#include "Camera.h"
using namespace BONE_GRAPHICS;

SkinnedMesh::SkinnedMesh(GameObject* gameObject) {
    this->gameObject = gameObject;
    SetTypeName("SkinnedMesh");
}

void SkinnedMesh::CalculateWorldMatrices(Bone *child, D3DXMATRIX *parentMatrix) {
    D3DXMatrixMultiply(&child->WorldMatrix, &child->TransformationMatrix, parentMatrix);

    //Each sibling has same parent as child
    //so pass the parent matrix of this child
    if (child->pFrameSibling) {
        CalculateWorldMatrices((Bone*)child->pFrameSibling, parentMatrix);
    }

    //Pass child matrix as parent to children
    if (child->pFrameFirstChild) {
        CalculateWorldMatrices((Bone*)child->pFrameFirstChild, &child->WorldMatrix);
    }
}

void SkinnedMesh::AddBoneMatrixPointers(Bone *bone) 
{
    if (bone->pMeshContainer != NULL) {
        BoneMesh* boneMesh = (BoneMesh*)bone->pMeshContainer;

        if (boneMesh->pSkinInfo != NULL) {
            //Get the bones affecting this mesh' skin.
            int nBones = boneMesh->pSkinInfo->GetNumBones();

            //Allocate memory for the pointer array
            boneMesh->boneMatrixPtrs = new D3DXMATRIX*[nBones];

            for (int i = 0; i<nBones; i++) {
                Bone* bone = (Bone*)D3DXFrameFind(m_pRootNode, boneMesh->pSkinInfo->GetBoneName(i));
                if (bone != NULL) {
                    boneMesh->boneMatrixPtrs[i] = &bone->WorldMatrix;
                }
                else {
                    boneMesh->boneMatrixPtrs[i] = NULL;
                }
            }
        }
    }

    //Traverse Hierarchy
    if (bone->pFrameSibling) { AddBoneMatrixPointers((Bone*)bone->pFrameSibling); }
    if (bone->pFrameFirstChild) { AddBoneMatrixPointers((Bone*)bone->pFrameFirstChild); }
}

void SkinnedMesh::SetFileName(std::string fileName)
{
    this->fileName = fileName;
}

std::string SkinnedMesh::GetFileName()
{
    return fileName;
}

void SkinnedMesh::LoadContent()
{
    AllocateHierarchyImp boneHierarchy;

    std::string FullPath;
    if (!ResourceMgr->ExistFile(fileName, &FullPath))
        return;
    
    if (SUCCEEDED(D3DXLoadMeshHierarchyFromX(FullPath.c_str(), D3DXMESH_MANAGED,
        RenderMgr->GetDevice(), &boneHierarchy,
        NULL, &m_pRootNode, &m_pAnimControl)))
    {
        D3DXMATRIX identity;
        D3DXMatrixIdentity(&identity);
        CalculateWorldMatrices((Bone*)m_pRootNode, &identity);

        AddBoneMatrixPointers((Bone*)m_pRootNode);

        GetAnimationSets();
    }
}

void SkinnedMesh::FreeBoneMatrixPointers(Bone *bone) {
    if (bone->pMeshContainer != NULL) {
        BoneMesh* boneMesh = (BoneMesh*)bone->pMeshContainer;

        if (boneMesh->boneMatrixPtrs != NULL) {
            delete[] boneMesh->boneMatrixPtrs;
        }
    }

    //Traverse Hierarchy
    if (bone->pFrameSibling) { FreeBoneMatrixPointers((Bone*)bone->pFrameSibling); }
    if (bone->pFrameFirstChild) { FreeBoneMatrixPointers((Bone*)bone->pFrameFirstChild); }
}

void SkinnedMesh::Render(Bone *bone) {
    //Call the function with NULL parameter to use root node
    if (bone == NULL) {
        bone = (Bone*)m_pRootNode;
    }

    //Check if a bone has a mesh associated with it
    if (bone->pMeshContainer != NULL)
    {
        BoneMesh *boneMesh = (BoneMesh*)bone->pMeshContainer;

        //Is there skin info?
        if (boneMesh->pSkinInfo != NULL)
        {
            //Get the number of bones influencing the skin
            //from pSkinInfo.
            int numInflBones = boneMesh->pSkinInfo->GetNumBones();
            for (int i = 0; i < numInflBones; i++)
            {
                //Get the local bone matrices, uneffected by parents
                D3DXMatrixMultiply(&boneMesh->localBoneMatrices[i],
                    &boneMesh->boneOffsetMatrices[i],
                    boneMesh->boneMatrixPtrs[i]);
            }

            //Upload bone matrices to shader.
            ResourceMgr->LoadEffect("Skinning.fx")->SetMatrixArray("BoneMatrices", boneMesh->localBoneMatrices, boneMesh->pSkinInfo->GetNumBones());


            D3DXMATRIX identity;
            D3DXMatrixIdentity(&identity);
            //D3DXMATRIX MatWorld = ((Transform3D*)gameObject->transform3D)->GetTransform();
            RenderMgr->GetDevice()->SetTransform(D3DTS_WORLD, &identity);

            //Render the mesh
            for (int i = 0; i < (int)boneMesh->NumAttributeGroups; i++)
            {
                //Use the attribute table to select material and texture attributes
                int mtrlIndex = boneMesh->attributeTable[i].AttribId;

                if (mtrlIndex < boneMesh->Materials.size()) {
                    auto mainCamera = SceneMgr->CurrentScene()->GetCurrentCamera();
                    auto camera = ((Camera*)mainCamera->GetComponent("Camera"));
                    D3DXVECTOR4 lightPos(-50.0f, 75.0f, -80.0f, 0.0f);

                    ResourceMgr->LoadEffect("Skinning.fx")->SetMatrix("matVP", 
                        &(camera->GetViewMatrix(mainCamera) * camera->GetProjectionMatrix())
                    );
                    ResourceMgr->LoadEffect("Skinning.fx")->SetVector("lightPos", &lightPos);

                    RenderMgr->GetDevice()->SetMaterial(&(boneMesh->Materials[mtrlIndex]));
                    RenderMgr->GetDevice()->SetTexture(0, boneMesh->Textures[mtrlIndex]);
                    ResourceMgr->LoadEffect("Skinning.fx")->SetMatrix("matWorld", &identity);
                    //Upload the texture to the shader
                    ResourceMgr->LoadEffect("Skinning.fx")->SetTexture("texDiffuse", boneMesh->Textures[mtrlIndex]);
                    D3DXHANDLE hTech = ResourceMgr->LoadEffect("Skinning.fx")->GetTechniqueByName("Skinning");
                    ResourceMgr->LoadEffect("Skinning.fx")->SetTechnique(hTech);
                    ResourceMgr->LoadEffect("Skinning.fx")->Begin(NULL, NULL);
                    ResourceMgr->LoadEffect("Skinning.fx")->BeginPass(0);

                    //Pass the index blended mesh to the technique
                    boneMesh->MeshData.pMesh->DrawSubset(mtrlIndex);

                    ResourceMgr->LoadEffect("Skinning.fx")->EndPass();
                    ResourceMgr->LoadEffect("Skinning.fx")->End();
                }
            }
        }
        else {

            //We have a static mesh; not animated.
            ResourceMgr->LoadEffect("Skinning.fx")->SetMatrix("matWorld", &bone->WorldMatrix);

            D3DXHANDLE hTech = ResourceMgr->LoadEffect("Skinning.fx")->GetTechniqueByName("Lighting");
            ResourceMgr->LoadEffect("Skinning.fx")->SetTechnique(hTech);

            //Render the subsets of this mesh with Lighting
            for (int mtrlIndex = 0; mtrlIndex<(int)boneMesh->Materials.size(); mtrlIndex++) {
                ResourceMgr->LoadEffect("Skinning.fx")->SetTexture("texDiffuse", boneMesh->Textures[mtrlIndex]);
                ResourceMgr->LoadEffect("Skinning.fx")->Begin(NULL, NULL);
                ResourceMgr->LoadEffect("Skinning.fx")->BeginPass(0);

                //Pass the index blended mesh to the technique
                boneMesh->MeshData.pMesh->DrawSubset(mtrlIndex);

                ResourceMgr->LoadEffect("Skinning.fx")->EndPass();
                ResourceMgr->LoadEffect("Skinning.fx")->End();
            }
        }
    }

    //Traverse the hierarchy; Rendering each mesh as we go
    if (bone->pFrameSibling != NULL) { Render((Bone*)bone->pFrameSibling); }
    if (bone->pFrameFirstChild != NULL) { Render((Bone*)bone->pFrameFirstChild); }
}

void SkinnedMesh::GetAnimationSets() {
    ID3DXAnimationSet* pAnim = NULL;

    for (int i = 0; i<(int)m_pAnimControl->GetMaxNumAnimationSets(); i++)
    {
        pAnim = NULL;
        m_pAnimControl->GetAnimationSet(i, &pAnim);

        //If we found an animation set, add it to the map
        if (pAnim != NULL)
        {
            string name = pAnim->GetName();
            animationSets[name] = i;//Creates an entry
            pAnim->Release();
        }
    }
}

void SkinnedMesh::SetAnimation(std::string name) {
    ID3DXAnimationSet* pAnim = NULL;
    //Get the animation set from the name.
    m_pAnimControl->GetAnimationSet(animationSets[name], &pAnim);

    if (pAnim != NULL)
    {
        //Set the current animation set
        m_pAnimControl->SetTrackAnimationSet(0, pAnim);
        pAnim->Release();
    }
}

void SkinnedMesh::PlayAnimation(float time) {

    static bool OnlyOne = true;
    if (OnlyOne)
    {
        SetAnimation("Skeleton_1H_banging_shield");
        OnlyOne = false;
    }

    //The world matrix here allows us to position the model in the scene.
    m_pAnimControl->AdvanceTime(time, NULL);//Second parameter not used.
                                            //Update the matrices that represent the pose of animation.

    D3DXMATRIX Mat;
    D3DXMatrixIdentity(&Mat);
    //D3DXMATRIX MatWorld = ((Transform3D*)gameObject->transform3D)->GetTransform();
    CalculateWorldMatrices((Bone*)m_pRootNode, &Mat);
}

SkinnedMesh::~SkinnedMesh() {
    FreeBoneMatrixPointers((Bone*)m_pRootNode);
}