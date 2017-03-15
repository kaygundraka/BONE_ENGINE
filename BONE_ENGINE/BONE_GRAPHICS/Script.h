#pragma once
#include "Common.h"
#include "Component.h"
#include "GameObject.h"
using namespace BONE_SYSTEM;

namespace BONE_GRAPHICS
{
    class Script : public Component {
    protected:
        GameObject* gameObject;
    
    public:
        Script(GameObject* gameObject, std::string name);
        virtual ~Script();

        virtual void Awake();
        virtual void Init();
        virtual void Reference();
        virtual void Update();
        virtual void LateUpdate();
        virtual void LateRender();
        
        virtual void OnCollisionEnter(GameObject* otherObject);
        virtual void OnCollisionStay(GameObject* otherObject);
        virtual void OnCollisionLeave(GameObject* otherObject);
    };
}