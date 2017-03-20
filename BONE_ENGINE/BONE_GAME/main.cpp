#include <Windows.h>
#include <Interface.h>
#include <SceneManager.h>
#include <Scene.h>
#include <BoneEditor.h>
#include "SetScript.h"
using namespace BONE_GRAPHICS;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
    HWND GameHWND;

    if (!FastCreateWindow(GameHWND, hInstance))
        return 0;

    if (!InitializeFramework(GameHWND, true))
        return 0;

    SceneMgr->SetGameObject = SetGameObject;
    RuntimeMgr->AddSource("TestScript2");

    BoneEditor Editor;
    Editor.SetScriptProc = SetScriptProc;
    Editor.Run();

    return 0;
}
