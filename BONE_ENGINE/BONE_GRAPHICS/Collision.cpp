#include "Common.h"
#include "Collision.h"
#include "LogManager.h"
#include "Transform3D.h"
#include "PhyscisManager.h"

namespace BONE_GRAPHICS {

    Collision::Collision(GameObject* attachObject)
    {
        collision = nullptr;
        type = NONE;

        object = attachObject;
        isShow = true;
    
        SetTypeName("Collision");
    }

    Collision::~Collision()
    {

    }

    void Collision::SetShow(bool render, D3DXCOLOR color)
    {
        isShow = render;
        this->color = color;
    }
    
    bool Collision::IsShow()
    {
        return isShow;
    }

    void Collision::Update()
    {
        auto Pivot = ((Transform3D*)object->transform3D)->GetPosition();

        if (type == COLLISION_TYPE::AABB)
            ((TAG_BOX*)collision)->pivot = Pivot;
        else if (type == COLLISION_TYPE::SPHERE)
            ((TAG_SPHERE*)collision)->pivot = Pivot;
        else if (type == COLLISION_TYPE::OBB)
            ((TAG_OBB*)collision)->pivot = Pivot;
    }

    void Collision::Render(GameObject* object)
    {
        if (isShow)
        {
            if (type == COLLISION_TYPE::AABB)
            {
                auto AABB = (TAG_BOX*)collision;

                auto Pivot = ((Transform3D*)object->transform3D)->GetPosition();

                Vector3 Vertex[8];
                
                Vertex[0].x = AABB->leftBottom.x;
                Vertex[0].y = AABB->leftBottom.y;
                Vertex[0].z = AABB->leftBottom.z;
                
                Vertex[1].x = AABB->rightTop.x;
                Vertex[1].y = AABB->leftBottom.y;
                Vertex[1].z = AABB->leftBottom.z;

                Vertex[2].x = AABB->leftBottom.x;
                Vertex[2].y = AABB->rightTop.y;
                Vertex[2].z = AABB->leftBottom.z;

                Vertex[3].x = AABB->leftBottom.x;
                Vertex[3].y = AABB->leftBottom.y;
                Vertex[3].z = AABB->rightTop.z;

                Vertex[4].x = AABB->rightTop.x;
                Vertex[4].y = AABB->rightTop.y;
                Vertex[4].z = AABB->leftBottom.z;

                Vertex[5].x = AABB->rightTop.x;
                Vertex[5].y = AABB->leftBottom.y;
                Vertex[5].z = AABB->rightTop.z;

                Vertex[6].x = AABB->leftBottom.x;
                Vertex[6].y = AABB->rightTop.y;
                Vertex[6].z = AABB->rightTop.z;

                Vertex[7].x = AABB->rightTop.x;
                Vertex[7].y = AABB->rightTop.y;
                Vertex[7].z = AABB->rightTop.z;

                for (int i = 0; i < 8; i++)
                    Vertex[i] += Pivot;

                RenderMgr->DrawLine(Vertex[0], Vertex[1], color);
                RenderMgr->DrawLine(Vertex[0], Vertex[2], color);
                RenderMgr->DrawLine(Vertex[0], Vertex[3], color);

                RenderMgr->DrawLine(Vertex[7], Vertex[4], color);
                RenderMgr->DrawLine(Vertex[7], Vertex[5], color);
                RenderMgr->DrawLine(Vertex[7], Vertex[6], color);

                RenderMgr->DrawLine(Vertex[2], Vertex[4], color);
                RenderMgr->DrawLine(Vertex[2], Vertex[6], color);

                RenderMgr->DrawLine(Vertex[5], Vertex[1], color);
                RenderMgr->DrawLine(Vertex[5], Vertex[3], color);
            }
        }
    }

    bool Collision::CreateAABB(Vector3 leftBottom, Vector3 rightTop, Vector3 pivot)
    {
        if (collision != nullptr)
            return false;

        collision = new TAG_BOX;
        ((TAG_BOX*)collision)->leftBottom = leftBottom;
        ((TAG_BOX*)collision)->rightTop = rightTop;
        ((TAG_BOX*)collision)->pivot = pivot;

        type = AABB;

        PhysicsMgr->AddCollision(this);

        return true;
    }

    bool Collision::CreateSphere(float radius, Vector3 pivot)
    {
        if (collision != nullptr)
            return false;

        collision = new TAG_SPHERE;
        ((TAG_SPHERE*)collision)->radius = radius;
        ((TAG_SPHERE*)collision)->pivot = pivot;

        type = SPHERE;

        PhysicsMgr->AddCollision(this);

        return true;
    }

    bool Collision::CreateOBB(Vector3 pivot, Vector3* axisDir, float *axisLen)
    {
        if (collision != nullptr)
            return false;

        collision = new TAG_OBB;
        ((TAG_OBB*)collision)->axisDir[0] = axisDir[0];
        ((TAG_OBB*)collision)->axisDir[1] = axisDir[1];
        ((TAG_OBB*)collision)->axisDir[2] = axisDir[2];
        ((TAG_OBB*)collision)->axisLen[0] = axisLen[0];
        ((TAG_OBB*)collision)->axisLen[1] = axisLen[1];
        ((TAG_OBB*)collision)->axisLen[2] = axisLen[2];
        ((TAG_OBB*)collision)->pivot = pivot;

        type = OBB;

        PhysicsMgr->AddCollision(this);

        return true;
    }

    bool Collision::ComputeBoundingBox(LPD3DXMESH mesh)
    {
        if (collision != nullptr)
            return false;

        collision = new TAG_BOX;
        type = AABB;

        LPVOID pVertices(nullptr);
        mesh->LockVertexBuffer(D3DLOCK_NOSYSLOCK, &pVertices);

        if (FAILED(D3DXComputeBoundingBox((Vector3*)pVertices,
            mesh->GetNumVertices(),
            D3DXGetFVFVertexSize(mesh->GetFVF()),
            &(((TAG_BOX*)collision)->leftBottom),
            &(((TAG_BOX*)collision)->rightTop))))
        {
            LogMgr->ShowMessage(LOG_ERROR, "Compute BoundingBox Failed..");
            mesh->UnlockVertexBuffer();

            this->Reset();

            return false;
        }

        mesh->UnlockVertexBuffer();

        PhysicsMgr->AddCollision(this);

        return true;
    }

    bool Collision::ComputeBoundingSphere(LPD3DXMESH mesh)
    {
        if (collision != nullptr)
            return false;

        collision = new TAG_SPHERE;
        type = SPHERE;

        LPVOID pVertices(nullptr);
        mesh->LockVertexBuffer(D3DLOCK_NOSYSLOCK, &pVertices);

        if (FAILED(D3DXComputeBoundingSphere((Vector3*)pVertices,
            mesh->GetNumVertices(),
            D3DXGetFVFVertexSize(mesh->GetFVF()),
            &(((TAG_SPHERE*)collision)->pivot),
            &(((TAG_SPHERE*)collision)->radius))))
        {
            LogMgr->ShowMessage(LOG_ERROR, "Compute BoundingShere Failed..");
            mesh->UnlockVertexBuffer();

            this->Reset();

            return false;
        }

        mesh->UnlockVertexBuffer();

        PhysicsMgr->AddCollision(this);

        return true;
    }

    bool Collision::ComputeBoundingSphere(LPD3DXFRAME _lpFrame)
    {
        if (collision != nullptr)
            return false;

        collision = new TAG_SPHERE;
        type = SPHERE;
        
        if (FAILED(D3DXFrameCalculateBoundingSphere(_lpFrame, &((TAG_SPHERE*)collision)->pivot, &((TAG_SPHERE*)collision)->radius)))
        {
            LogMgr->ShowMessage(LOG_ERROR, "Compute BoundingShere Failed..");

            return false;
        }

        PhysicsMgr->AddCollision(this);

        return true;
    }

    /*TAG_OBB Collision::ConvertAABBtoOBB(const TAG_BOX& box, TAG_OBB obb)
    {
        obb.pivot = (box.leftBottom + box.rightTop) / 2.0f;

        Vector3 X(box.rightTop.x, box.leftBottom.y, box.leftBottom.z);
        Vector3 Y(box.leftBottom.x, box.rightTop.y, box.leftBottom.z);
        Vector3 Z(box.leftBottom.x, box.leftBottom.y, box.rightTop.z);

        obb.axisDir[0] = X - box.leftBottom;
        D3DXVec3Normalize(&obb.axisDir[0], &obb.axisDir[0]);

        obb.axisDir[1] = Y - box.leftBottom;
        D3DXVec3Normalize(&obb.axisDir[1], &obb.axisDir[1]);

        obb.axisDir[2] = Z - box.leftBottom;
        D3DXVec3Normalize(&obb.axisDir[2], &obb.axisDir[2]);

        obb.axisLen[0] = D3DXVec3Length(&obb.axisDir[0]) * 0.5f;
        obb.axisLen[1] = D3DXVec3Length(&obb.axisDir[1]) * 0.5f;
        obb.axisLen[2] = D3DXVec3Length(&obb.axisDir[2]) * 0.5f;

        return obb;
    }*/

    GameObject* Collision::GetGameObject()
    {
        return object;
    }

    Collision::COLLISION_TYPE Collision::GetCollisionType()
    {
        return type;
    }

    COLLISION* Collision::GetCollision()
    {
        return collision;
    }

    void Collision::Reset()
    {
        if (collision != nullptr)
        {
            PhysicsMgr->RemoveCollision(this);

            delete collision;
            collision = nullptr;
            type = NONE;
        }
    }
}