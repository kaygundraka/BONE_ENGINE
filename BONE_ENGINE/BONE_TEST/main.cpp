#include <Windows.h>
#include <Interface.h>
#include <SceneManager.h>
#include <Scene.h>

//#include "CameraObject.h"
//#include "TestMesh.h"

using namespace BONE_GRAPHICS;

class GUI : public GUI_Scene {
private:
    bool open;

public:
    GUI() {
        open = true;
    }

    void ShowFileMenu()
    {
        if (ImGui::MenuItem("New")) {}
        if (ImGui::MenuItem("Open", "Ctrl+O")) {}
        if (ImGui::MenuItem("Save", "Ctrl+S")) {}
        if (ImGui::MenuItem("Save As..")) {}
        if (ImGui::MenuItem("Quit", "Alt+F4")) {}
    }

    void OptionFileMenu()
    {
        if (ImGui::BeginMenu("RenderMode"))
        {
            if (ImGui::MenuItem("POLYGON")) {}
            if (ImGui::MenuItem("WIRE_FRAME")) {}

            ImGui::EndMenu();
        }
    }

    virtual void UpdateFrame()
    {
        if (!ImGui::Begin("Prefab Editor", &open, ImGuiWindowFlags_MenuBar))
        {
            ImGui::End();
            return;
        }

        //ImGui::PushItemWidth(-140);
        ImGui::Text("Dear ImGui says hello.");

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                ShowFileMenu();
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Option"))
            {
                OptionFileMenu();
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ImGui::End();
    }
};

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
    //ShowCursor(false);

    HWND GameHWND;

    if (!FastCreateWindow(GameHWND, hInstance))
        return 0;

    if (!InitializeFramework(GameHWND, true))
        return 0;

    bool flag = false;

    auto_ptr<GUI> gui(new GUI);

    {
        auto_ptr<Scene> TestScene(new Scene);
        SceneMgr->SetGUIScene(gui.get());
        //auto_ptr<CameraObject> Camera(new CameraObject);
        //auto_ptr<TestMesh> TestMesh(new TestMesh);

        //TestScene->SetAmbientColor(0.1f, 0.1f, 0.1f, 0.1f);
        //TestScene->SetSkybox("violentdays", "tga");
        //TestScene->AddObject(Camera.get(), "Camera");
        //TestScene->AddObject(TestMesh.get(), "TestMesh");
        
        SceneMgr->AddScene("TestScene", TestScene.get());
        flag = SceneMgr->StartScene("TestScene");
    }

    return 0;
}