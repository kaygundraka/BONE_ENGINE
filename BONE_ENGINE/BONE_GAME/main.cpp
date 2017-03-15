#include <Windows.h>
#include <Interface.h>
#include <SceneManager.h>
#include <Scene.h>
#include <BoneEditor.h>

using namespace BONE_GRAPHICS;

bool AddScriptHandler(GameObject* object, std::string name)
{
    return true;
}

void SetScriptProc(BoneEditor* editor)
{
    editor->AddScript = AddScriptHandler;

    editor->AddScriptList("TestScript");
    editor->AddScriptList("TestScript2");
    editor->AddScriptList("TestScript3");
    editor->AddScriptList("TestScript4");
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
    HWND GameHWND;

    if (!FastCreateWindow(GameHWND, hInstance))
        return 0;

    if (!InitializeFramework(GameHWND, true))
        return 0;

    BoneEditor Editor;
    Editor.SetScriptProc = SetScriptProc;
    Editor.Run();

    return 0;
}


/*if (IsNewScene)
{
auto_ptr<PointLight> Light(new PointLight);

Light->SetAmbient(1.0f, 1.0f, 1.0f, 1.0f);
Light->SetDiffuse(1.0f, 1.0f, 1.0f, 1.0f);
Light->SetSpecular(0.5f, 0.5f, 0.5f, 0.5f);
Light->SetRadius(200);
Light->SetLight(true);
Light->SetPrfabName("PointLight");
Light->SetPosition(Vector3(0, 200, 0));

ViewScene->AddObject(Light.get(), "PointLight");
}
*/