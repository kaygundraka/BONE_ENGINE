#include "Common.h"
#include "RenderManager.h"
#include "LogManager.h"

namespace BONE_GRAPHICS
{
    struct line_vertex
    {
        float x, y, z;
        DWORD color;
    };

    void RenderManager::DrawLine(D3DXMATRIX matrix, D3DXVECTOR3 start, D3DXVECTOR3 end, D3DXCOLOR color)
    {
        D3D_DEVICE->SetTransform(D3DTS_WORLD, &matrix);

        line_vertex line_vertices[2];

        const DWORD line_fvf = D3DFVF_XYZ | D3DFVF_DIFFUSE;

        line_vertices[0].x = start.x;
        line_vertices[0].y = start.y;
        line_vertices[0].z = start.z;
        line_vertices[0].color = color;

        line_vertices[1].x = end.x;
        line_vertices[1].y = end.y;
        line_vertices[1].z = end.z;
        line_vertices[1].color = color;

        D3D_DEVICE->SetFVF(line_fvf);
        D3D_DEVICE->SetRenderState(D3DRS_LIGHTING, false);
        D3D_DEVICE->SetTexture(0, NULL);

        D3D_DEVICE->DrawPrimitiveUP(D3DPT_LINELIST, 1, line_vertices, sizeof(line_vertex));
    }

    void RenderManager::DrawLine(D3DXVECTOR3 start, D3DXVECTOR3 end, D3DXCOLOR color)
    {
        Matrix Transform;
        D3DXMatrixIdentity(&Transform);
        D3D_DEVICE->SetTransform(D3DTS_WORLD, &Transform);

        line_vertex line_vertices[2];

        const DWORD line_fvf = D3DFVF_XYZ | D3DFVF_DIFFUSE;

        line_vertices[0].x = start.x;
        line_vertices[0].y = start.y;
        line_vertices[0].z = start.z;
        line_vertices[0].color = color;

        line_vertices[1].x = end.x;
        line_vertices[1].y = end.y;
        line_vertices[1].z = end.z;
        line_vertices[1].color = color;

        D3D_DEVICE->SetFVF(line_fvf);

        D3D_DEVICE->SetTexture(0, NULL);

        D3D_DEVICE->DrawPrimitiveUP(D3DPT_LINELIST, 1, line_vertices, sizeof(line_vertex));
    }

    void RenderManager::DrawBox(D3DXMATRIX transform, D3DXVECTOR3 leftBottom, D3DXVECTOR3 rightTop, D3DXCOLOR color)
    {
        D3DXVECTOR3 Vertex[8];

        Vertex[0].x = leftBottom.x;
        Vertex[0].y = leftBottom.y;
        Vertex[0].z = leftBottom.z;

        Vertex[1].x = rightTop.x;
        Vertex[1].y = leftBottom.y;
        Vertex[1].z = leftBottom.z;

        Vertex[2].x = leftBottom.x;
        Vertex[2].y = rightTop.y;
        Vertex[2].z = leftBottom.z;

        Vertex[3].x = leftBottom.x;
        Vertex[3].y = leftBottom.y;
        Vertex[3].z = rightTop.z;

        Vertex[4].x = rightTop.x;
        Vertex[4].y = rightTop.y;
        Vertex[4].z = leftBottom.z;

        Vertex[5].x = rightTop.x;
        Vertex[5].y = leftBottom.y;
        Vertex[5].z = rightTop.z;

        Vertex[6].x = leftBottom.x;
        Vertex[6].y = rightTop.y;
        Vertex[6].z = rightTop.z;

        Vertex[7].x = rightTop.x;
        Vertex[7].y = rightTop.y;
        Vertex[7].z = rightTop.z;

        this->DrawLine(transform, Vertex[0], Vertex[1], color);
        this->DrawLine(transform, Vertex[0], Vertex[2], color);
        this->DrawLine(transform, Vertex[0], Vertex[3], color);

        this->DrawLine(transform, Vertex[7], Vertex[4], color);
        this->DrawLine(transform, Vertex[7], Vertex[5], color);
        this->DrawLine(transform, Vertex[7], Vertex[6], color);

        this->DrawLine(transform, Vertex[2], Vertex[4], color);
        this->DrawLine(transform, Vertex[2], Vertex[6], color);

        this->DrawLine(transform, Vertex[5], Vertex[1], color);
        this->DrawLine(transform, Vertex[5], Vertex[3], color);

        this->DrawLine(transform, Vertex[1], Vertex[4], color);
        this->DrawLine(transform, Vertex[3], Vertex[6], color);
    }

    void RenderManager::DrawSphere(D3DXMATRIX transform, float radius, D3DXCOLOR color)
    {
        LPD3DXMESH mesh;
        D3DXCreateSphere(D3D_DEVICE, radius, 12, 0, &mesh, 0);

        D3D_DEVICE->SetTransform(D3DTS_WORLD, &transform);
        DWORD RenderState;
        D3D_DEVICE->GetRenderState(D3DRS_FILLMODE, &RenderState);
        D3D_DEVICE->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

        D3D_DEVICE->SetTexture(0, NULL);
        mesh->DrawSubset(0);

        D3D_DEVICE->SetRenderState(D3DRS_FILLMODE, RenderState);

        mesh->Release();
    }

    void RenderManager::DrawCylinder(D3DXMATRIX transform, float radius, float height, D3DXCOLOR color)
    {
        const DWORD line_fvf = D3DFVF_XYZ | D3DFVF_DIFFUSE;

        IDirect3DVertexBuffer9* VertexBuffer;
        
        line_vertex* pVertex;

        D3D_DEVICE->CreateVertexBuffer(79 * sizeof(line_vertex), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, line_fvf, D3DPOOL_DEFAULT, &VertexBuffer, NULL);

        WORD wVertexIndex = 0;

        int nCurrentSegment;
        //Lock the vertex buffer
        VertexBuffer->Lock(0, 0, (VOID**)&pVertex, D3DLOCK_NOSYSLOCK | D3DLOCK_DISCARD);
                
        float rDeltaSegAngle = (2.0f * D3DX_PI / 12);
        float rSegmentLength = 1.0f / (float)12;

        //Create the sides triangle strip
        for (nCurrentSegment = 0; nCurrentSegment <= 12; nCurrentSegment++) {
            float x0 = radius * sinf(nCurrentSegment * rDeltaSegAngle);
            float z0 = radius * cosf(nCurrentSegment * rDeltaSegAngle);
            pVertex->x = x0;
            pVertex->y = 0.0f + (height / 2.0f);
            pVertex->z = z0;
            pVertex++;
            wVertexIndex++;
            pVertex->x = x0;
            pVertex->y = 0.0f - (height / 2.0f);
            pVertex->z = z0;
            pVertex++;
            wVertexIndex++;
        }
        
        //Create the top triangle fan: Center
        pVertex->x = 0.0f;
        pVertex->y = 0.0f + (height / 2.0f);
        pVertex->z = 0.0f;
        pVertex++;
        wVertexIndex++;

        //Create the top triangle fan: Edges
        for (nCurrentSegment = 0; nCurrentSegment <= 12; nCurrentSegment++) {
            float x0 = radius * sinf(nCurrentSegment * rDeltaSegAngle);
            float z0 = radius * cosf(nCurrentSegment * rDeltaSegAngle);
            pVertex->x = x0;
            pVertex->y = 0.0f + (height / 2.0f);
            pVertex->z = z0;
            pVertex++;
            wVertexIndex++;
        }

        //Create the bottom triangle fan: Center
        pVertex->x = 0.0f;
        pVertex->y = 0.0f - (height / 2.0f);
        pVertex->z = 0.0f;
        pVertex++;
        wVertexIndex++;

        //Create the bottom triangle fan: Edges
        for (nCurrentSegment = 12; nCurrentSegment >= 0; nCurrentSegment--) {
            float x0 = radius * sinf(nCurrentSegment * rDeltaSegAngle);
            float z0 = radius * cosf(nCurrentSegment * rDeltaSegAngle);
            pVertex->x = x0;
            pVertex->y = 0.0f - (height / 2.0f);
            pVertex->z = z0;
            pVertex++;
            wVertexIndex++;
        }

        VertexBuffer->Unlock();

        D3D_DEVICE->SetTransform(D3DTS_WORLD, &transform);

        DWORD RenderState;
        D3D_DEVICE->GetRenderState(D3DRS_FILLMODE, &RenderState);
        D3D_DEVICE->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

        D3D_DEVICE->SetFVF(line_fvf);

        D3D_DEVICE->SetTexture(0, NULL);
        D3D_DEVICE->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, wVertexIndex / 3, (void*)pVertex, sizeof(line_vertex));

        D3D_DEVICE->SetRenderState(D3DRS_FILLMODE, RenderState);

        VertexBuffer->Release();
    }

    void RenderManager::DrawCone(D3DXMATRIX transform, float edge, float edge2, float edge3, D3DXCOLOR color)
    {

    }

    bool RenderManager::DrawMeshBox(D3DXMATRIX matrix, LPD3DXMESH mesh, D3DXVECTOR3 pos, D3DXCOLOR color)
    {
        D3DXVECTOR3 LeftBottom;
        D3DXVECTOR3 RightTop;

        LPVOID pVertices(nullptr);
        mesh->LockVertexBuffer(D3DLOCK_NOSYSLOCK, &pVertices);

        if (FAILED(D3DXComputeBoundingBox((Vec3*)pVertices,
            mesh->GetNumVertices(),
            D3DXGetFVFVertexSize(mesh->GetFVF()),
            &LeftBottom, &RightTop)))
        {
            LogMgr->Error("Compute BoundingBox Failed..");
            mesh->UnlockVertexBuffer();

            return false;
        }

        mesh->UnlockVertexBuffer();

        D3DXVECTOR3 Vertex[8];

        Vertex[0].x = LeftBottom.x;
        Vertex[0].y = LeftBottom.y;
        Vertex[0].z = LeftBottom.z;

        Vertex[1].x = RightTop.x;
        Vertex[1].y = LeftBottom.y;
        Vertex[1].z = LeftBottom.z;

        Vertex[2].x = LeftBottom.x;
        Vertex[2].y = RightTop.y;
        Vertex[2].z = LeftBottom.z;

        Vertex[3].x = LeftBottom.x;
        Vertex[3].y = LeftBottom.y;
        Vertex[3].z = RightTop.z;

        Vertex[4].x = RightTop.x;
        Vertex[4].y = RightTop.y;
        Vertex[4].z = LeftBottom.z;

        Vertex[5].x = RightTop.x;
        Vertex[5].y = LeftBottom.y;
        Vertex[5].z = RightTop.z;

        Vertex[6].x = LeftBottom.x;
        Vertex[6].y = RightTop.y;
        Vertex[6].z = RightTop.z;

        Vertex[7].x = RightTop.x;
        Vertex[7].y = RightTop.y;
        Vertex[7].z = RightTop.z;

        this->DrawLine(matrix, Vertex[0], Vertex[1], color);
        this->DrawLine(matrix, Vertex[0], Vertex[2], color);
        this->DrawLine(matrix, Vertex[0], Vertex[3], color);

        this->DrawLine(matrix, Vertex[7], Vertex[4], color);
        this->DrawLine(matrix, Vertex[7], Vertex[5], color);
        this->DrawLine(matrix, Vertex[7], Vertex[6], color);

        this->DrawLine(matrix, Vertex[2], Vertex[4], color);
        this->DrawLine(matrix, Vertex[2], Vertex[6], color);

        this->DrawLine(matrix, Vertex[5], Vertex[1], color);
        this->DrawLine(matrix, Vertex[5], Vertex[3], color);

        this->DrawLine(matrix, Vertex[1], Vertex[4], color);
        this->DrawLine(matrix, Vertex[3], Vertex[6], color);

        return true;
    }
}