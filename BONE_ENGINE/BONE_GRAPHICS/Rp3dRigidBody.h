#pragma once
#include "Common.h"
#include "Component.h"
#include "GameObject.h"
#include "Transform3D.h"

namespace BONE_GRAPHICS
{
    class RigidBody : public Component {
    private:
        rp3d::RigidBody* rigidBody;
        GameObject* owner;
        Transform3D* transform;
        rp3d::Transform rp3dTransform;

        Vec3 pos;

        bool lockRotateX;
        bool lockRotateY;
        bool lockRotateZ;

        float mass;

    public:
        RigidBody();
        virtual ~RigidBody();

        bool SetInfo(GameObject* owner, float mass);

        void SetType(reactphysics3d::BodyType type = reactphysics3d::BodyType::DYNAMIC);
        void EnableGravity(bool enable = false);
        void SetBounciness(float value = 0.4f);
        void SetFrictionCoefficient(float value = 0.2f);
        void SetIsAllowedToSleep(bool value = false);
        void SetMass(float mass);

        reactphysics3d::BodyType GetType();
        bool IsEnableGravity();
        bool GetIsAllowedToSleep();
        float GetBounciness();
        float GetFrictionCoefficient();
        float GetMass();
        Vector3 GetLinearVelocity();

        void LockUpdate();
        void LockRotation(bool x, bool y, bool z);
        bool IsLockedRotationX();
        bool IsLockedRotationY();
        bool IsLockedRotationZ();

        void SetAngularVelocity(float x, float y, float z);
        void SetLinearVelocity(float x, float y, float z);

        void SetPosOnPivot(Vec3 pos);
        Vec3 GetPosOnPivot();

        void AddForce(Vec3 force);
        void AddForce(rp3d::Vector3 force, rp3d::Vector3 point);
        void ApplyTorque(rp3d::Vector3 torque);
        void ApplyTorque(Vec3 torque);
        void SetTransform(Vec3 Translate, Vec3 Rotate);

        void UpdateTransform();
    };
}
