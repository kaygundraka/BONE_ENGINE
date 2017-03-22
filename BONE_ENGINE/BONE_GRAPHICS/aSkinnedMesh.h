#pragma once
#include "Common.h"
#include "AllocateHierarchyImp.h"
#include "Component.h"
#include "GameObject.h"

struct Bone : public D3DXFRAME
{
    D3DXMATRIX WorldMatrix;
};

struct BoneMesh : public D3DXMESHCONTAINER
{
    vector<D3DMATERIAL9> Materials;
    vector<IDirect3DTexture9*> Textures;

    DWORD NumAttributeGroups;
    D3DXATTRIBUTERANGE* attributeTable;
    D3DXMATRIX** boneMatrixPtrs;
    D3DXMATRIX* boneOffsetMatrices;
    D3DXMATRIX* localBoneMatrices;
};

namespace BONE_GRAPHICS
{
    class SkinnedMesh : public Component {
    public:
        SkinnedMesh(GameObject* gameObject);
        ~SkinnedMesh();
        
        void SetFileName(std::string fileName);
        std::string GetFileName();

        virtual void LoadContent();
        void Render(Bone* bone);

    private:
        std::string fileName;

        void CalculateWorldMatrices(Bone* child, D3DXMATRIX* parentMatrix);
        void AddBoneMatrixPointers(Bone* bone);
        void FreeBoneMatrixPointers(Bone* bone);

        void GetAnimationSets();

        GameObject* gameObject;
        D3DXFRAME* m_pRootNode;
        ID3DXAnimationController* m_pAnimControl;
        map<std::string, int> animationSets;

    public:
        void SetAnimation(std::string name);
        void PlayAnimation(float time);
    };
}