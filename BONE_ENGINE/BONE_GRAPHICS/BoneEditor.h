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
        bool showAddComponent;
        bool showEnvironmentSetting;
        bool showLogWindow;
        bool showMainEditor;

        std::string currentShowInfoObject;

        bool isFocused;

        int childSize;

        LogDialog logDialog;

        std::string currentObjectName;
        std::string rootObjectName;

        std::list<std::string> scriptList;
        std::string playScene;
        bool isTestPlay;
        
        bool isEditMode;

    public:
        BoneEditor();

        void SaveScene();

        void ShowFileMenu();
        void ShowEditorMenu();
        void ShowViewMenu();
        void ShowHelpMenu();

        void ShowGraphNodeInfo(GraphNode* node);

        void SelectObject(std::string name);

        void ShowObjectInfo(std::string name);
        void ShowEditObjectTree(std::string treeName);

        bool IsEditMode();

        virtual void UpdateFrame();

        void AddScriptList(std::string scriptName);
        std::list<std::string> GetScriptList();

        void(*SetScriptProc)(BoneEditor* editor);
        bool(*AddScript)(GameObject* object, std::string scripts);

        void Run();
    };
}