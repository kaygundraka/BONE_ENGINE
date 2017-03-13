#include <Windows.h>
#include <Interface.h>
#include <SceneManager.h>
#include <Scene.h>
#include <GameObject.h>
#include <Transform3D.h>
#include <PointLight.h>
#include <StaticMesh.h>
#include <InputManager.h>

#include "EditorUI.h"
#include "CameraObject.h"

using namespace BONE_GRAPHICS;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
    HWND GameHWND;

    if (!FastCreateWindow(GameHWND, hInstance))
        return 0;

    if (!InitializeFramework(GameHWND, true))
        return 0;

    bool flag = false;

    RenderMgr->GetDevice()->SetRenderState(D3DRS_LIGHTING, FALSE);
    InputMgr->SetFocusWindow(true);

    auto_ptr<EditorUI> gui(new EditorUI);
    SceneMgr->SetGUIScene(gui.get());

    {
        auto_ptr<Scene> ViewScene(new Scene);

        auto_ptr<PointLight> Light(new PointLight);
        Light->SetAmbient(1.0f, 1.0f, 1.0f, 1.0f);
        Light->SetDiffuse(1.0f, 1.0f, 1.0f, 1.0f);
        Light->SetSpecular(0.5f, 0.5f, 0.5f, 0.5f);
        Light->SetRadius(200);
        Light->SetLight(true);
        Light->SetPrfabName("PointLight");
        Light->SetPosition(Vector3(0, 200, 0));

        auto_ptr<CameraObject> MainCamera(new CameraObject);

        ViewScene->AddObject(MainCamera.get(), "EditorCamera");
        ViewScene->AddObject(Light.get(), "PointLight");
        ViewScene->SetAmbientColor(1.0f, 1.0f, 1.0f, 1.0f);
        
        SceneMgr->AddScene("ViewScene", ViewScene.get());
        flag = SceneMgr->StartScene("ViewScene");
    }

    return 0;
}