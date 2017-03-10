#include <Windows.h>
#include <Interface.h>
#include <SceneManager.h>
#include <Scene.h>

#include "CameraObject.h"
#include "TestMesh.h"

using namespace BONE_GRAPHICS;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
    ShowCursor(false);

    HWND GameHWND;

    if (!FastCreateWindow(GameHWND, hInstance))
        return 0;

    if (!InitializeFramework(GameHWND))
        return 0;

    bool flag = false;

    {
        auto_ptr<Scene> TestScene(new Scene);
        auto_ptr<CameraObject> Camera(new CameraObject);
        auto_ptr<TestMesh> TestMesh(new TestMesh);

        TestScene->SetAmbientColor(0.1f, 0.1f, 0.1f, 0.1f);
        TestScene->SetSkybox("violentdays", "tga");
        TestScene->AddObject(Camera.get(), "Camera");
        TestScene->AddObject(TestMesh.get(), "TestMesh");
        
        SceneMgr->AddScene("TestScene", TestScene.get());
        flag = SceneMgr->StartScene("TestScene");
    }

    return 0;
}