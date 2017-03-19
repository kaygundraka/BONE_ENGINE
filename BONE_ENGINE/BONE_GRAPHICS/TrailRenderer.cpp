#include "Common.h"
#include "RenderManager.h"
#include "LogManager.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "GameObject.h"
#include "TrailRenderer.h"
#include "Vertex.h"

namespace BONE_GRAPHICS
{
    TrailRenderer::TrailRenderer()
    {
        SetTypeName("TrailRenderer");

        vertexBuffer = nullptr;
        indexBuffer = nullptr;

        liveCycle = 1;
        createCycle = 2;
        curCreateTime = 0;

        pivot = Vec3(0, 0, 0);
        upDir = Vec3(0, 1, 0);

        startSize = 1;
        endSize = 1;

        renderMode = RENDER_ALPHA;

        IsActive = true;
        IsInit = false;
    }

    TrailRenderer::~TrailRenderer()
    {
        if (vertexBuffer != nullptr)
            vertexBuffer->Release();

        if (indexBuffer != nullptr)
            indexBuffer->Release();
    }

    void TrailRenderer::LoadContent()
    {
        IsInit = true;
    }

    void TrailRenderer::SetTargetObject(GameObject* targetObject, Vec3 pivot)
    {
        target = targetObject;
        this->pivot = pivot;
    }

    void TrailRenderer::SetSize(float start, float end)
    {
        startSize = start;
        endSize = end;
    }

    void TrailRenderer::SetCycle(float liveCycle, float createCycle)
    {
        if (liveCycle < 0)
            this->liveCycle = 0;
        else
            this->liveCycle = liveCycle;

        if (createCycle < 0)
            this->createCycle = 0;
        else
            this->createCycle = createCycle;
    }

    void TrailRenderer::UpdateStatus()
    {
        for (auto Iter = trailList.begin(); Iter != trailList.end(); Iter++)
        {
            if (liveCycle < (*Iter).curLiveTime)
                Iter = trailList.erase(Iter);
            else
            {
                float time = (*Iter).curLiveTime / liveCycle;

                (*Iter).curSize = (-(startSize + endSize)) * time + (startSize + endSize) + endSize;
                (*Iter).curLiveTime += SceneMgr->GetTimeDelta();
            }
        }

        if (IsActive)
        {
            if (createCycle < curCreateTime)
            {
                TRAIL_MESH temp;
                temp.curLiveTime = 0;
                temp.curSize = startSize;

                Vec3 Posit = pivot;
                D3DXVec3TransformCoord(&Posit, &Posit, &((Transform3D*)target->GetComponent("Transform3D"))->GetTransform());
                temp.pivot = Posit;

                Vec3 UpDir = upDir;
                D3DXVec3TransformNormal(&UpDir, &UpDir, &((Transform3D*)target->GetComponent("Transform3D"))->GetTransform());
                D3DXVec3Normalize(&UpDir, &UpDir);
                temp.upDir = UpDir;

                trailList.push_back(temp);

                curCreateTime = 0;
            }

            curCreateTime += SceneMgr->GetTimeDelta();

            if (trailList.size() >= 2)
                SetMeshBuffer();

        }
    }

    void TrailRenderer::SetMeshBuffer()
    {
        VERTEX* vertex;
        vertex = new VERTEX[trailList.size() * 2];

        int index = 0;

        for (int i = trailList.size() - 1; i >= 0; i--)
        {
            vertex[index].p = trailList[i].pivot + (trailList[i].upDir * trailList[i].curSize / 2);
            vertex[index].n = vertex[index].p;
            D3DXVec3Normalize(&vertex[index].n, &vertex[index].n);
            vertex[index].uv.x = trailList.size() - 1 - i;
            vertex[index++].uv.y = 0;

            vertex[index].p = trailList[i].pivot - (trailList[i].upDir * trailList[i].curSize / 2);
            vertex[index].n = vertex[index].p;
            D3DXVec3Normalize(&vertex[index].n, &vertex[index].n);
            vertex[index].uv.x = trailList.size() - 1 - i;
            vertex[index++].uv.y = 1;
        }

        if (vertexBuffer != nullptr)
        {
            vertexBuffer->Release();
            vertexBuffer = nullptr;
        }

        if (FAILED(RenderMgr->GetDevice()->CreateVertexBuffer(
            trailList.size() * 2 * sizeof(VERTEX), D3DUSAGE_WRITEONLY,
            VERTEX::FVF, D3DPOOL_DEFAULT, &vertexBuffer, nullptr)))
        {
            return;
        }

        VOID* pVertices;

        if (FAILED(vertexBuffer->Lock(0, trailList.size() * 2 * sizeof(VERTEX), (void**)&pVertices, 0)))
            return;

        VERTEX* pV = (VERTEX*)pVertices;

        for (int i = 0; i < trailList.size() * 2; i++)
            *(pV++) = vertex[i];

        vertexBuffer->Unlock();

        delete[] vertex;

        if (indexBuffer != nullptr)
        {
            indexBuffer->Release();
            indexBuffer = nullptr;
        }

        if (FAILED(RenderMgr->GetDevice()->CreateIndexBuffer(
            (trailList.size() * 2 - 2) * sizeof(VERTEX_INDEX),
            0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &indexBuffer, nullptr)))
            return;

        VERTEX_INDEX IB;
        int IB_Index = 0;
        VOID* pVertices2;

        if (FAILED(indexBuffer->Lock(0, (trailList.size() * 2 - 2) * sizeof(VERTEX_INDEX), (void**)&pVertices2, 0)))
            return;

        VERTEX_INDEX* pI = (VERTEX_INDEX*)pVertices2;

        for (int i = 0; i < (trailList.size() * 2 - 2) / 2; i++)
        {
            IB._0 = IB_Index;
            IB._1 = IB_Index + 1;
            IB._2 = IB_Index + 2;
            *(pI++) = IB;

            IB._0 = IB_Index + 2;
            IB._1 = IB_Index + 1;
            IB._2 = IB_Index + 3;
            *(pI++) = IB;

            IB_Index += 2;
        }

        indexBuffer->Unlock();
    }

    void TrailRenderer::SetUpVector(Vec3 upDir)
    {
        this->upDir = upDir;
        D3DXVec3Normalize(&this->upDir, &this->upDir);
    }

    void TrailRenderer::SetActive(bool active)
    {
        IsActive = active;
    }

    void TrailRenderer::SetRenderMode(int mode)
    {
        renderMode = mode;
    }

    void TrailRenderer::Render(IShader* shaderOpt)
    {
        if (IsInit && trailList.size() >= 2 && IsActive)
        {
            if (renderMode == RENDER_ALPHA)
            {
                RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
                RenderMgr->GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
                RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHAREF, 0);

                RenderMgr->GetDevice()->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTA_TEXTURE);
            }
            else if (renderMode == RENDER_STENCIL)
            {
                RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
                RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHAREF, 0x00);
                RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

            }

            Matrix World;
            D3DXMatrixIdentity(&World);
            RenderMgr->GetDevice()->SetTransform(D3DTS_WORLD, &World);
            RenderMgr->GetDevice()->SetStreamSource(0, vertexBuffer, 0, sizeof(VERTEX));
            RenderMgr->GetDevice()->SetIndices(indexBuffer);
            RenderMgr->GetDevice()->SetTexture(0, ResourceMgr->LoadTexture(textureAddress));
            RenderMgr->GetDevice()->SetFVF(VERTEX::FVF);
            RenderMgr->GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0,
                trailList.size() * 2, 0, (trailList.size() - 1) * 2);

            if (renderMode == RENDER_ALPHA)
                RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
            else if (renderMode == RENDER_STENCIL)
                RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, false);
        }
    }

    void TrailRenderer::SetTexturesAddress(std::string address)
    {
        textureAddress = address;
    }

    std::string TrailRenderer::GetTexturesAddress()
    {
        return textureAddress;
    }

    Vec3 TrailRenderer::GetPivot()
    {
        return pivot;
    }

    float TrailRenderer::GetStartSize()
    {
        return startSize;
    }
    
    float TrailRenderer::GetEndSize()
    {
        return endSize;
    }
    
    float TrailRenderer::GetLiveCycle()
    {
        return liveCycle;
    }
    
    float TrailRenderer::GetCreateCycle()
    {
        return createCycle;
    }
    
    Vec3 TrailRenderer::GetUpVector()
    {
        return upDir;
    }
    
    bool TrailRenderer::Activated()
    {
        return IsActive;
    }
    
    TrailRenderer::RENDER_MODE TrailRenderer::GetRenderMode()
    {
        return (RENDER_MODE)renderMode;
    }
}