#pragma once
#include "Common.h"
#include "Component.h"
#include "GameObject.h"
using namespace BONE_SYSTEM;

namespace BONE_GRAPHICS
{
    class Script : public Component {
    public:
        Script();
        virtual ~Script();

        void SetInfo(GameObject* gameObject, std::string name);

        GameObject* gameObject;

        virtual void Awake();
        virtual void Init();
        virtual void Reference();
        virtual void Update();
        virtual void LateUpdate();
        virtual void LateRender();
        
        virtual void CollisionEvent(GameObject* otherObject);
    };
}