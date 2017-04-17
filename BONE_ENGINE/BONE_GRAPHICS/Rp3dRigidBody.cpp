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
        if (SceneMgr->CurrentScene() != nullptr && rigidBody != nullptr)
            SceneMgr->CurrentScene()->GetPhysicsWorld()->destroyRigidBody(rigidBody);
    }

    bool RigidBody::SetInfo(GameObject* owner, float mass)
    {
        this->owner = owner;
        
        if (this->owner->GetComponent("Collision") == nullptr)
            return false;

        this->mass = mass;
        
        transform = (Transform3D*)this->owner->transform3D;
        
        rp3d::Vector3 InitPosition(transform->GetWorldPositon().x, transform->GetWorldPositon().y, transform->GetWorldPositon().z);
        rp3d::Quaternion InitOrientation(transform->GetWorldRotateAngle().x, transform->GetWorldRotateAngle().z, transform->GetWorldRotateAngle().z);
        rp3d::Transform InitTransform(InitPosition, InitOrientation);

        rigidBody = SceneMgr->CurrentScene()->GetPhysicsWorld()->createRigidBody(InitTransform);

        rp3d::Vector3 InitPosition2(0, 0, 0);
        rp3d::Quaternion InitOrientation2(0, 0, 0);
        rp3d::Transform InitTransform2(InitPosition2, InitOrientation2);

        rigidBody->addCollisionShape(
            ((Collision*)this->owner->GetComponent("Collision"))->GetCollision(),
            InitTransform2,
            mass
        );

        pos = ((Collision*)this->owner)->GetModelPivot();

        lockRotateX = false;
        lockRotateY = false;
        lockRotateZ = false;

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

    void RigidBody::SetMass(float mass)
    {
        rigidBody->setMass(mass);
    }

    void RigidBody::AddForce(Vec3 force)
    {
        rp3d::Vector3 Force(force.x, force.y, force.z);
        
        rigidBody->applyForceToCenterOfMass(Force);
    }
    
    void RigidBody::AddForce(rp3d::Vector3 force, rp3d::Vector3 point)
    {
        rigidBody->applyForce(force, point);
    }

    void RigidBody::SetTransform(Vec3 Translate, Vec3 Rotate)
    {
        reactphysics3d::Transform transform;

        transform.setPosition(reactphysics3d::Vector3(Translate.x, Translate.y, -Translate.z));

        reactphysics3d::Quaternion NewQuater(Rotate.x, Rotate.y, Rotate.z);
        transform.setOrientation(NewQuater);

        rigidBody->setTransform(transform);
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

    void RigidBody::SetAngularVelocity(float x, float y, float z)
    {
        rigidBody->setAngularVelocity(Vector3(x, y, z));
    }

    void RigidBody::SetLinearVelocity(float x, float y, float z)
    {
        rigidBody->setLinearVelocity(Vector3(x, y, z));
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

    Vector3 RigidBody::GetLinearVelocity()
    {
        return rigidBody->getLinearVelocity();
    }

    void RigidBody::SetPosOnPivot(Vec3 pos)
    {
        this->pos = pos;
    }

    Vec3 RigidBody::GetPosOnPivot()
    {
        return pos;
    }

    void RigidBody::LockRotation(bool x, bool y, bool z)
    {
        lockRotateX = x;
        lockRotateY = y;
        lockRotateZ = z;
    }
    
    bool RigidBody::IsLockedRotationX()
    {
        return lockRotateX;
    }
    
    bool RigidBody::IsLockedRotationY()
    {
        return lockRotateY;
    }
    
    bool RigidBody::IsLockedRotationZ()
    {
        return lockRotateZ;
    }

    void RigidBody::LockUpdate()
    {
        Vector3 AngularAxis = rigidBody->getAngularVelocity();

        if (lockRotateX)
            AngularAxis.x = 0;

        if (lockRotateY)
            AngularAxis.y = 0;

        if (lockRotateZ)
            AngularAxis.z = 0;

        rigidBody->setAngularVelocity(AngularAxis);
    }
    
    void RigidBody::UpdateTransform()
    {
        if (SceneMgr->CurrentScene()->IsEnablePhysics())
        {
            rp3dTransform = rigidBody->getTransform();
            rp3d::Quaternion Rot = rp3dTransform.getOrientation();
            rp3d::Vector3 Pos = rp3dTransform.getPosition();

            auto CurPos = transform->GetWorldPositon() - transform->GetPosition();
            transform->SetPosition(Pos.x - pos.x - CurPos.x, Pos.y - pos.y - CurPos.y, (-Pos.z) - pos.z - CurPos.z);

            Quater quater;
            quater.x = Rot.x;
            quater.y = -Rot.y;
            quater.z = Rot.z;
            quater.w = -Rot.w;

            auto CurQuater = transform->GetWorldRotateQuater() - transform->GetRotateQuater();
            quater -= CurQuater;
            transform->SetRotate(quater);
        }
        else
        {
            rp3d::Vector3 InitPosition(transform->GetWorldPositon().x + pos.x, transform->GetWorldPositon().y + pos.y, -(transform->GetWorldPositon().z + pos.z));
            rp3d::Quaternion InitOrientation(transform->GetWorldRotateAngle().x, transform->GetWorldRotateAngle().y, transform->GetWorldRotateAngle().z);
            rp3d::Transform InitTransform(InitPosition, InitOrientation);

            rigidBody->setTransform(InitTransform);
        }
    }
}