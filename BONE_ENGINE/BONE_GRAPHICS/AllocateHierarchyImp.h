#pragma once
#include "Common.h"

class AllocateHierarchyImp : public ID3DXAllocateHierarchy {
public:
    STDMETHOD(CreateFrame)(LPCSTR Name,
        LPD3DXFRAME* ppNewFrame);

    STDMETHOD(CreateMeshContainer)(LPCSTR Name,
        CONST D3DXMESHDATA* pMeshData,
        CONST D3DXMATERIAL* pMaterials,
        CONST D3DXEFFECTINSTANCE* pEffectInstances,
        DWORD NumMaterials,
        CONST DWORD* pAdjacency,
        LPD3DXSKININFO pSkinInfo,
        LPD3DXMESHCONTAINER* ppNewMeshContainer);

    STDMETHOD(DestroyFrame)(LPD3DXFRAME pFrameToFree);

    STDMETHOD(DestroyMeshContainer)(LPD3DXMESHCONTAINER pMeshContainerBase);
};