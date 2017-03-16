#pragma once
#include "Common.h"
#include "SceneManager.h"

namespace BONE_GRAPHICS
{
    class SceneInfoUI : public GUI_Scene {
    private:
        bool open;
        bool openPopup;
        bool endScene;

        bool isNewScene;
        std::string openSceneName;

    public:
        SceneInfoUI();

        virtual void UpdateFrame();

        bool IsNewScene();
        std::string GetSceneName();
    };
}