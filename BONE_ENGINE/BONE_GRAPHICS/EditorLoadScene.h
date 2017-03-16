#pragma once
#include "Common.h"
#include "SceneManager.h"

namespace BONE_GRAPHICS
{
    class EditorLoadScene : public GUI_Scene {
    private:
        bool open;
        float progress;
        float temp;

    public:
        EditorLoadScene();

        virtual void UpdateFrame();
    };
}