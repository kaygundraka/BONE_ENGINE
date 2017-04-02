#include "Common.h"
#include "Rp3dCollision.h"
#include "LogManager.h"
#include "SceneManager.h"

namespace BONE_GRAPHICS
{
    Collision::Collision(GameObject* attachObject)
    {
        object = attachObject;
        
        collision = nullptr;
        
        transform = (Transform3D*)((this->object)->transform3D);

        this->SetTypeName("Collision");
        type = COLL_CAPSULE;

        showShape = false;
    }

    Collision::~Collision()
    {
        if (body != nullptr)
            SceneMgr->CurrentScene()->GetPhysicsWorld()->destroyCollisionBody(body);

        if (collision != nullptr)
            delete collision;
    }
    
    bool Collision::CreateBox(Vector3 halfExtens)
    {
        if (collision != nullptr)
            return false;

        rp3d::Vector3 Half(halfExtens.x, halfExtens.y, halfExtens.z);

        collision = new BoxShape(Half);
        collision->name = object->GetName();

        type = COLL_BOX;
        this->halfExtens = Vec3(halfExtens.x, halfExtens.y, halfExtens.z);
    }

    bool Collision::CreateSphere(float radius)
    {
        if (collision != nullptr)
            return false;

        this->radius = radius;

        collision = new rp3d::SphereShape(radius);
        collision->name = object->GetName();

        type = COLL_SPHERE;
    }

    bool Collision::CreateCone(float radius, float height)
    {
        if (collision != nullptr)
            return false;

        this->radius = radius;
        this->height = height;

        collision = new rp3d::ConeShape(radius, height);
        collision->name = object->GetName();
        
        type = COLL_CONE;
    }

    bool Collision::CreateCylinder(float radius, float height)
    {
        if (collision != nullptr)
            return false;

        this->radius = radius;
        this->height = height;

        collision = new rp3d::CylinderShape(radius, height);
        collision->name = object->GetName();

        type = COLL_CYLINDER;
    }
    
    bool Collision::CreateCapsule(float radius, float height)
    {
        if (collision != nullptr)
            return false;
    
        this->radius = radius;
        this->height = height;

        collision = new rp3d::CapsuleShape(radius, height);
        collision->name = object->GetName();

        type = COLL_CAPSULE;
    }

    bool Collision::ComputeBoundingBox(LPD3DXMESH mesh)
    {
        if (collision != nullptr)
            return false;

        TAG_BOX Box;
        
        LPVOID pVertices(nullptr);
        mesh->LockVertexBuffer(D3DLOCK_NOSYSLOCK, &pVertices);
                
        if (FAILED(D3DXComputeBoundingBox((Vec3*)pVertices,
            mesh->GetNumVertices(),
            D3DXGetFVFVertexSize(mesh->GetFVF()),
            &(Box.leftBottom),
            &(Box.rightTop))))
        {
            LogMgr->Error("Compute BoundingBox Failed..");
            mesh->UnlockVertexBuffer();

            return false;
        }

        mesh->UnlockVertexBuffer();

        Box.rightTop.x *= ((Transform3D*)object->transform3D)->GetScale().x;
        Box.rightTop.y *= ((Transform3D*)object->transform3D)->GetScale().y;
        Box.rightTop.z *= ((Transform3D*)object->transform3D)->GetScale().z;

        Box.leftBottom.x *= ((Transform3D*)object->transform3D)->GetScale().x;
        Box.leftBottom.y *= ((Transform3D*)object->transform3D)->GetScale().y;
        Box.leftBottom.z *= ((Transform3D*)object->transform3D)->GetScale().z;

        Vec3 HalfExtens = (Box.rightTop - Box.leftBottom) / 2;
        pivot = (Box.rightTop + Box.leftBottom) / 2;

        this->halfExtens = HalfExtens;
       
        rp3d::Vector3 Half(HalfExtens.x, HalfExtens.y, HalfExtens.z);

        collision = new BoxShape(Half);
        collision->name = object->GetName();

        type = COLL_BOX;

        return true;
    }

    Vec3 Collision::GetModelPivot()
    {
        return pivot;
    }
    
    void Collision::SetModelPivot(Vec3 pivot)
    {
        this->pivot = pivot;
    }

    bool Collision::ComputeBoundingSphere(LPD3DXMESH mesh)
    {
        if (collision != nullptr)
            return false;

        TAG_SPHERE Sphere;

        LPVOID pVertices(nullptr);
        mesh->LockVertexBuffer(D3DLOCK_NOSYSLOCK, &pVertices);

        if (FAILED(D3DXComputeBoundingSphere((Vec3*)pVertices,
            mesh->GetNumVertices(),
            D3DXGetFVFVertexSize(mesh->GetFVF()),
            &(Sphere.pivot),
            &(Sphere.radius))))
        {
            LogMgr->Error("Compute BoundingShere Failed..");
            mesh->UnlockVertexBuffer();

            return false;
        }

        mesh->UnlockVertexBuffer();

        this->radius = Sphere.radius;

        pivot = Sphere.pivot;
        
        collision = new SphereShape(Sphere.radius);
        collision->name = object->GetName();

        type = COLL_SPHERE;

        return true;
    }
    
    bool Collision::ComputeBoundingSphere(LPD3DXFRAME frameMesh)
    {
        if (collision != nullptr)
            return false;

        TAG_SPHERE Sphere;

        if (FAILED(D3DXFrameCalculateBoundingSphere(frameMesh, &(Sphere.pivot), &(Sphere.radius))))
        {
            LogMgr->Error("Compute BoundingShere Failed..");

            return false;
        }

        this->radius = Sphere.radius;
        pivot = Sphere.pivot;

        collision = new SphereShape(Sphere.radius);
        collision->name = object->GetName();
     
        type = COLL_SPHERE;

        return true;
    }

    void Collision::ShowShape(bool show)
    {
        showShape = show;
    }
    
    bool Collision::IsShow()
    {
        return showShape;
    }

    void Collision::RenderShape()
    {
        if (showShape)
        {
            if (type == COLLISION_TYPE::COLL_BOX)
            {
                Vec3 LeftBottom = -halfExtens;
                Vec3 RightTop = halfExtens;
                RenderMgr->DrawBox(
                    ((Transform3D*)object->transform3D)->GetTransform(),
                    LeftBottom + pivot,
                    RightTop + pivot,
                    COLOR::GREEN
                );
            }
            else if (type == COLLISION_TYPE::COLL_SPHERE)
            {
                RenderMgr->DrawSphere(
                    ((Transform3D*)object->transform3D)->GetTransform(),
                    radius,
                    COLOR::GREEN
                );
            }
            else if (type == COLLISION_TYPE::COLL_CYLINDER)
            {
                RenderMgr->DrawCylinder(
                    ((Transform3D*)object->transform3D)->GetTransform(),
                    radius,
                    height,
                    COLOR::GREEN
                );
            }
            else if (type == COLLISION_TYPE::COLL_CAPSULE)
            {

            }
            else if (type == COLLISION_TYPE::COLL_CONE)
            {

            }
        }
    }
    
    Vec3 Collision::GetHalfExtens()
    {
        return halfExtens;
    }
    
    float Collision::GetRadius()
    {
        return radius;
    }
    
    float Collision::GetHeight()
    {
        return height;
    }

    ConvexShape* Collision::GetCollision()
    {
        return collision;
    }

    GameObject* Collision::GetGameObject()
    {
        return this->object;
    }

    Collision::COLLISION_TYPE Collision::GetCollisionType()
    {
        return type;
    }
}