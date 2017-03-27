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
        GameObject* onwer;
        Transform3D* transform;
        rp3d::Transform rp3dTransform;

        Vec3 pos;
        float mass;

    public:
        RigidBody();
        virtual ~RigidBody();

        bool SetInfo(GameObject* onwer, float mass);

        void SetType(reactphysics3d::BodyType type = reactphysics3d::BodyType::DYNAMIC);
        void EnableGravity(bool enable = false);
        void SetBounciness(float value = 0.4f);
        void SetFrictionCoefficient(float value = 0.2f);
        void SetIsAllowedToSleep(bool value = false);
        void SetMass(float mass);

        reactphysics3d::BodyType GetType();
        bool IsEnableGravity();
        float GetBounciness();
        float GetFrictionCoefficient();
        bool GetIsAllowedToSleep();
        float GetMass();


        void SetPosOnPivot(Vec3 pos);
        Vec3 GetPosOnPivot();

        void AddForce(Vec3 force, Vec3 point);
        void ADDForce(rp3d::Vector3 force, rp3d::Vector3 point);
        void ApplyTorque(rp3d::Vector3 torque);
        void ApplyTorque(Vec3 torque);

        void UpdateTransform();
    };
}
