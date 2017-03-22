#include "Common.h"
#include "AllocateHierarchyImp.h"
#include "SkinnedMesh.h"
#include "ResourceManager.h"
#pragma warning (disable:4996)
using namespace BONE_GRAPHICS;

HRESULT AllocateHierarchyImp::CreateFrame(LPCSTR Name, LPD3DXFRAME *ppNewFrame)
{
    Bone *bone = new Bone;
    memset(bone, 0, sizeof(Bone));

    if (Name != NULL)
    {
        //Allocate memory for name
        bone->Name = new char[strlen(Name)];
        strcpy(bone->Name, Name);
    }

    //Prepare Matrices
    D3DXMatrixIdentity(&bone->TransformationMatrix);
    D3DXMatrixIdentity(&bone->WorldMatrix);

    //Return the new bone
    *ppNewFrame = (D3DXFRAME*)bone;

    return S_OK;
}

HRESULT AllocateHierarchyImp::DestroyFrame(LPD3DXFRAME pFrameToFree)
{
    if (pFrameToFree)
    {
        //Free up memory
        if (pFrameToFree->Name != NULL)
            delete[] pFrameToFree->Name;

        delete pFrameToFree;
    }
    pFrameToFree = NULL;

    return S_OK;
}

HRESULT AllocateHierarchyImp::CreateMeshContainer(LPCSTR Name, CONST D3DXMESHDATA *pMeshData,
    CONST D3DXMATERIAL *pMaterials,
    CONST D3DXEFFECTINSTANCE *pEffectInstances,
    DWORD NumMaterials,
    CONST DWORD *pAdjacency,
    LPD3DXSKININFO pSkinInfo,
    LPD3DXMESHCONTAINER *ppNewMeshContainer)
{
    //Allocate memory for the new bone mesh
    //and initialize it to zero
    BoneMesh *boneMesh = new BoneMesh;
    memset(boneMesh, 0, sizeof(BoneMesh));

    //Add a reference to the mesh so the load function doesn't get rid of 	//it
    pMeshData->pMesh->AddRef();
    //Get the device
    IDirect3DDevice9 *pDevice = NULL;
    pMeshData->pMesh->GetDevice(&pDevice);

    //Get the mesh materials and create related textures
    D3DXMATERIAL mtrl;
    for (int i = 0; i<NumMaterials; i++) {
        memcpy(&mtrl, &pMaterials[i], sizeof(D3DXMATERIAL));

        boneMesh->Materials.push_back(mtrl.MatD3D);

        IDirect3DTexture9* pTexture = NULL;
        //If there is a texture associated with this material, load it into
        //the program
        if (mtrl.pTextureFilename != NULL) {
            boneMesh->Textures.push_back(ResourceMgr->LoadTexture(mtrl.pTextureFilename));
        }
        else {
            //Make sure we have the same number of elements in 				//Textures as we do Materials
            boneMesh->Textures.push_back(NULL);
        }
    }

    //Now we need to prepare the mesh for hardware skinning; as 	
    //mentioned earlier we need the bone offset matrices, and these are
    //stored in pSkinInfo.  Here we get the bone offset matrices and 	
    //allocate memory for the local bone matrices that influence the 	
    //mesh.  But of course this is only if skinning info is available.
    if (pSkinInfo != NULL) {
        boneMesh->pSkinInfo = pSkinInfo;
        pSkinInfo->AddRef();

        DWORD maxVertInfluences = 0;
        DWORD numBoneComboEntries = 0;
        ID3DXBuffer* boneComboTable = 0;

        //Convert mesh to indexed blended mesh to add additional 			
        //vertex components; weights and influencing bone indices.
        //Store the new mesh in the bone mesh.
        pSkinInfo->ConvertToIndexedBlendedMesh(pMeshData->pMesh,
            D3DXMESH_MANAGED | D3DXMESH_WRITEONLY,
            30,
            0, //Not used
            0, //Not used
            0, //Not used
            0, //Not used
            &maxVertInfluences,
            &numBoneComboEntries,
            &boneComboTable,
            &boneMesh->MeshData.pMesh);

        if (boneComboTable != NULL) //Not used
            boneComboTable->Release();

        //As mentioned, the attribute table is used for selecting 				
        //materials and textures to render on the mesh.  So we aquire 			
        //it here.
        boneMesh->MeshData.pMesh->GetAttributeTable(NULL, &boneMesh->NumAttributeGroups);
        boneMesh->attributeTable = new D3DXATTRIBUTERANGE[boneMesh->NumAttributeGroups];
        boneMesh->MeshData.pMesh->GetAttributeTable(boneMesh->attributeTable, NULL);

        //Next we load the offset matrices and allocate memory for 			
        //the local bone matrices.  skin info holds the number of bones 		
        //that influence this mesh in terms of the bones used to create 		
        //the skin.
        int NumBones = pSkinInfo->GetNumBones();
        boneMesh->boneOffsetMatrices = new D3DXMATRIX[NumBones];
        boneMesh->localBoneMatrices = new D3DXMATRIX[NumBones];

        for (int i = 0; i < NumBones; i++) {
            boneMesh->boneOffsetMatrices[i] = *(boneMesh->pSkinInfo->GetBoneOffsetMatrix(i));
        }
    }
    else {
        //We have a static mesh
        boneMesh->MeshData.pMesh = pMeshData->pMesh;
        boneMesh->MeshData.Type = pMeshData->Type;
    }

    //Return new mesh
    *ppNewMeshContainer = boneMesh;
    return S_OK;
}

HRESULT AllocateHierarchyImp::DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase)
{
    BoneMesh* boneMesh = (BoneMesh*)pMeshContainerBase;

    //Release textures
    int nElements = boneMesh->Textures.size();
    for (int i = 0; i<nElements; i++) {
        if (boneMesh->Textures[i] != NULL)
            ResourceMgr->ReleaseTexture(boneMesh->Textures[i]);
    }


    //Delete local bone matrices and offset if we have skin info
    if (boneMesh->pSkinInfo != NULL) {
        delete[] boneMesh->localBoneMatrices;
        delete[] boneMesh->boneOffsetMatrices;
        delete[] boneMesh->attributeTable;
    }

    //Release mesh and skin info
    if (boneMesh->pSkinInfo) { boneMesh->pSkinInfo->Release(); }
    if (boneMesh->MeshData.pMesh) { boneMesh->MeshData.pMesh->Release(); }

    return S_OK;
}