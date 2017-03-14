#pragma once
#include <Common.h>
#include <SceneManager.h>
using namespace BONE_GRAPHICS;

class EditorUI : public GUI_Scene {
private:
    bool open;
    bool showObjectInfo;
    bool showAddComponent;
    bool showPrefabHierarchical;

    std::string currentShowInfoObject;

    int childSize;

    std::string currentObjectName;
    std::string rootObjectName;

public:
    EditorUI();

    void ShowFileMenu();
    void ShowOpitionMenu();
    void ShowHelpMenu();

    void ShowObjectInfo(std::string name);
    void ShowGameObjectTree(std::string treeName);

    void AllChildCheck(GameObject* parent);

    virtual void UpdateFrame();
};