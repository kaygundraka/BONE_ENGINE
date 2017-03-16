#pragma once
#include "Common.h"
#include "SceneManager.h"
#include "ISingleton.h"

namespace BONE_GRAPHICS 
{
    class LogDialog
    {
    private:
        ImGuiTextBuffer     Buf;
        ImGuiTextFilter     Filter;
        ImVector<int>       LineOffsets;        // Index to lines offset
        bool                ScrollToBottom;

    public:
        void Clear();
        void AddLog(const char* fmt, ...) IM_PRINTFARGS(2);
        void Render(const char* title, bool* p_open = NULL);
    };

    class BoneEditor : public GUI_Scene {
    private:
        bool showObjectInfo;
        bool showAddComponent;
        bool showEnvironmentSetting;
        bool showPrefabInspector;
        bool showLogWindow;
        bool showMainEditor;

        std::string currentShowInfoObject;

        int childSize;

        LogDialog logDialog;

        std::string currentObjectName;
        std::string rootObjectName;

        std::list<std::string> scriptList;
        std::string playScene;
        bool isTestPlay;

    public:
        BoneEditor();

        void SaveScene();

        void ShowFileMenu();
        void ShowEditorMenu();
        void ShowHelpMenu();

        void SelectObject(std::string name);

        void ShowObjectInfo(std::string name);
        void ShowGameObjectTree(std::string treeName);

        void AllChildCheck(GameObject* parent);

        virtual void UpdateFrame();

        void AddScriptList(std::string scriptName);
        std::list<std::string> GetScriptList();

        void(*SetScriptProc)(BoneEditor* editor);
        bool(*AddScript)(GameObject* object, std::string scripts);

        void Run();
    };
}