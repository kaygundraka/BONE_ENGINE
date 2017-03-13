#pragma once
#include <Common.h>
#include <SceneManager.h>
using namespace BONE_GRAPHICS;

class SceneInfoUI : public GUI_Scene {
private:
    bool open;
    
public:
    SceneInfoUI();

    virtual void UpdateFrame();
};