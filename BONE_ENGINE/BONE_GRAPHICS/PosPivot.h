#pragma once
#include "StaticMesh.h"
#include "Script.h"
#include "BoneEditor.h"

namespace BONE_GRAPHICS
{
    class PosPivot : public Script {
    private:
        std::string selectObject;
        GameObject* mainCamera;
        
        bool isShow;

        bool moveX;
        bool moveY;
        bool moveZ;
        float moveValue;

        BoneEditor* ui;
        
    public:
        PosPivot(BoneEditor* ui, GameObject* gameObject, std::string name);
        virtual ~PosPivot();

        virtual void Init();
        virtual void Reference();
        virtual void Update();
    };
}