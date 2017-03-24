#include "Common.h"
#include "Rp3dRigidBody.h"
#include "SceneManager.h"
#include "Rp3dCollision.h"

namespace BONE_GRAPHICS
{
    RigidBody::RigidBody()
    {
        SetTypeName("RigidBody");
    }

    RigidBody::~RigidBody()
    {
    }

    bool RigidBody::SetInfo(GameObject* onwer, float mass)
    {
        this->onwer = onwer;
        
        if (this->onwer->GetComponent("Collision") == nullptr)
            return false;

        this->mass = mass;
        
        transform = (Transform3D*)this->onwer->transform3D;
        
        rp3d::Vector3 InitPosition(transform->GetPosition().x, transform->GetPosition().y, transform->GetPosition().z);
        rp3d::Quaternion InitOrientation(transform->GetRotateAngle().x, transform->GetRotateAngle().z, transform->GetRotateAngle().z);
        rp3d::Transform InitTransform(InitPosition, InitOrientation);

        rigidBody = SceneMgr->CurrentScene()->GetPhysicsWorld()->createRigidBody(InitTransform);

        rp3d::Vector3 InitPosition2(0, 0, 0);
        rp3d::Quaternion InitOrientation2(0, 0, 0);
        rp3d::Transform InitTransform2(InitPosition2, InitOrientation2);

        rigidBody->addCollisionShape(
            ((Collision*)this->onwer->GetComponent("Collision"))->GetCollision(),
            InitTransform2,
            mass
        );

        return true;
    }

    void RigidBody::SetType(reactphysics3d::BodyType type)
    {
        rigidBody->setType(type);
    }

    void RigidBody::EnableGravity(bool enable)
    {
        rigidBody->enableGravity(enable);
    }
    
    void RigidBody::SetBounciness(float value)
    {
        rp3d::Material& material = rigidBody->getMaterial();
        material.setBounciness(value);
    }
    
    void RigidBody::SetFrictionCoefficient(float value)
    {
        rp3d::Material& material = rigidBody->getMaterial();
        material.setFrictionCoefficient(value);
    }
    
    void RigidBody::SetIsAllowedToSleep(bool value)
    {
        rigidBody->setIsAllowedToSleep(value);
    }

    void RigidBody::AddForce(Vec3 force, Vec3 point)
    {
        rp3d::Vector3 Force(force.x, force.y, force.z);
        rp3d::Vector3 Point(point.x, point.y, point.z);

        rigidBody->applyForce(Force, Point);
    }
    
    void RigidBody::ADDForce(rp3d::Vector3 force, rp3d::Vector3 point)
    {
        rigidBody->applyForce(force, point);
    }

    void RigidBody::ApplyTorque(Vec3 torque)
    {
        rp3d::Vector3 Torque(torque.x, torque.y, torque.z);
        rigidBody->applyTorque(Torque);
    }
    
    void RigidBody::ApplyTorque(rp3d::Vector3 torque)
    {
        rigidBody->applyTorque(torque);
    }

    reactphysics3d::BodyType RigidBody::GetType()
    {
        return rigidBody->getType();
    }
    
    bool RigidBody::IsEnableGravity()
    {
        return rigidBody->isGravityEnabled();
    }
    
    float RigidBody::GetBounciness()
    {
        return rigidBody->getMaterial().getBounciness();
    }
    
    float RigidBody::GetFrictionCoefficient()
    {
        return rigidBody->getMaterial().getFrictionCoefficient();
    }
    
    bool RigidBody::GetIsAllowedToSleep()
    {
        return rigidBody->isAllowedToSleep();
    }

    float RigidBody::GetMass()
    {
        return mass;
    }

    void RigidBody::SetPosOnPivot(Vec3 pos)
    {
        this->pos = pos;
    }

    Vec3 RigidBody::GetPosOnPivot()
    {
        return pos;
    }

    void RigidBody::UpdateTransform()
    {
        if (SceneMgr->CurrentScene()->IsEnablePhysics())
        {
            rp3dTransform = rigidBody->getTransform();
            rp3d::Quaternion Rot = rp3dTransform.getOrientation();
            rp3d::Vector3 Pos = rp3dTransform.getPosition();
            
            transform->SetPosition(Pos.x + pos.x, Pos.y + pos.y, -Pos.z - pos.z);
            transform->SetRotate(Rot.x, Rot.y, Rot.z);
        }
        else
        {
            rp3d::Vector3 InitPosition(transform->GetPosition().x, transform->GetPosition().y, -transform->GetPosition().z);
            rp3d::Quaternion InitOrientation(transform->GetRotateAngle().x, transform->GetRotateAngle().z, transform->GetRotateAngle().z);
            rp3d::Transform InitTransform(InitPosition, InitOrientation);

            rigidBody->setTransform(InitTransform);
        }
    }
}