#include <Windows.h>
#include <Interface.h>
#include <SceneManager.h>
#include <Scene.h>
#include <BoneEditor.h>
#include <RuntimeCompiler.h>
#include <Script.h>
#include "TestScript2.h"
using namespace BONE_GRAPHICS;

namespace BONE_GRAPHICS {
    void Init(GameObject* owner)
    {
        auto scripts = owner->GetScript();

        for each(auto var in scripts)
        {
            if (var == "TestScript2")
            {
                ((TestScript2*)RuntimeMgr->GetSource("TestScript2"))->gameObject = owner;
                ((TestScript2*)RuntimeMgr->GetSource("TestScript2"))->Init();
            }
        }
    }

    void Awake(GameObject* owner)
    {
        auto scripts = owner->GetScript();

        for each(auto var in scripts)
        {
            if (var == "TestScript2")
            {
                ((TestScript2*)RuntimeMgr->GetSource("TestScript2"))->gameObject = owner;
                ((TestScript2*)RuntimeMgr->GetSource("TestScript2"))->Awake();
            }
        }
    }

    void Reference(GameObject* owner)
    {
        auto scripts = owner->GetScript();

        for each(auto var in scripts)
        {
            if (var == "TestScript2")
            {
                ((TestScript2*)RuntimeMgr->GetSource("TestScript2"))->gameObject = owner;
                ((TestScript2*)RuntimeMgr->GetSource("TestScript2"))->Reference();
            }
        }
    }

    void Update(GameObject* owner)
    {
        auto scripts = owner->GetScript();
        
        for each(auto var in scripts)
        {
            if (var == "TestScript2")
            {
                auto temp = ((Script*)RuntimeMgr->GetSource("TestScript2"));
                temp->SetInfo(owner, var);
                temp->Update();
            }
        }
    }

    void LateUpdate(GameObject* owner)
    {
        auto scripts = owner->GetScript();

        for each(auto var in scripts)
        {
            if (var == "TestScript2")
            {
                ((TestScript2*)RuntimeMgr->GetSource("TestScript2"))->gameObject = owner;
                ((TestScript2*)RuntimeMgr->GetSource("TestScript2"))->LateUpdate();
            }
        }
    }

    void LateRender(GameObject* owner)
    {
        auto scripts = owner->GetScript();

        for each(auto var in scripts)
        {
            if (var == "TestScript2")
            {
                ((TestScript2*)RuntimeMgr->GetSource("TestScript2"))->gameObject = owner;
                ((TestScript2*)RuntimeMgr->GetSource("TestScript2"))->LateRender();
            }
        }
    }

    void CollisionEvent(GameObject* owner, GameObject* otherObject)
    {
        auto scripts = owner->GetScript();

        for each(auto var in scripts)
        {
            if (var == "TestScript2")
            {
                ((TestScript2*)RuntimeMgr->GetSource("TestScript2"))->gameObject = owner;
                ((TestScript2*)RuntimeMgr->GetSource("TestScript2"))->CollisionEvent(otherObject);
            }
        }
    }

    void SetGameObject(GameObject* obj)
    {
        obj->InitFunc = Init;
        obj->AwakeFunc = Awake;
        obj->ReferenceFunc = Reference;
        obj->UpdateFunc = Update;
        obj->LateUpdateFunc = LateUpdate;
        obj->LateRenderFunc = LateRender;
        obj->CollisionEventFunc = CollisionEvent;
    }
}

bool AddScriptHandler(GameObject* object, std::string name)
{
    if (name == "TestScript2")
        object->AddScript(name);

    return true;
}

void SetScriptProc(BoneEditor* editor)
{
    editor->AddScript = AddScriptHandler;
    editor->AddScriptList("TestScript2");
}

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
