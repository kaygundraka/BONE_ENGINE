#pragma once
#include "Common.h"

using namespace std;

namespace BONE_GRAPHICS
{
    struct Bone : public D3DXFRAME {
        D3DXMATRIX CombinedTransformationMatrix;
    };

    struct BoneMesh : public D3DXMESHCONTAINER {
        ID3DXMesh* OriginalMesh;				// ��Ű�� ��꿡 ���� ���� �޽�
        vector<D3DMATERIAL9> materials;			// ���� ���
        vector<LPDIRECT3DTEXTURE9> diffuseTextures;	// �ؽ�ó ���
        vector<LPDIRECT3DTEXTURE9> normalTextures;

        DWORD NumAttributeGroups;				// �Ӽ� �׷��� ��
        D3DXATTRIBUTERANGE* attributeTables;	// �Ӽ� ���̺�
        D3DXMATRIX** boneMatrixPtrs;			// �� ��ȯ ��ĵ��� ������
        D3DXMATRIX*	boneOffsetMatrices;			// �� ������ ��ĵ�
        D3DXMATRIX* currentBoneMatrices;		// ������ �ջ�� ��ȯ ��ĵ�

        LPD3DXBUFFER pBoneCombinationBuf;
    };

    class BoneHierarchyLoader : public ID3DXAllocateHierarchy
    {
    public:
        STDMETHOD(CreateFrame) (THIS_ LPCSTR Name, LPD3DXFRAME *ppNewFrame);
        STDMETHOD(CreateMeshContainer) (THIS_ LPCSTR Name,
            CONST D3DXMESHDATA *pMeshData,
            CONST D3DXMATERIAL *pMaterials,
            CONST D3DXEFFECTINSTANCE *pEffectInstance,
            DWORD NumMaterials,
            CONST DWORD *pAdjacency,
            LPD3DXSKININFO pSkinInfo,
            LPD3DXMESHCONTAINER *ppNewMeshContainer);
        STDMETHOD(DestroyFrame) (THIS_ LPD3DXFRAME pFrameToFree);
        STDMETHOD(DestroyMeshContainer) (THIS_ LPD3DXMESHCONTAINER pMeshContainerBase);
    };
}