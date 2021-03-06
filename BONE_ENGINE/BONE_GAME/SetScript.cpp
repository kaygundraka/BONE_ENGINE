#include "SetScript.h"
#include "TestScript2.h"
#include "PlayerCharacter.h"
#include "GameManager.h"
#include "PlayerGUI.h"
#include "Monster.h"

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
    else if (name == "PlayerCharacter")
    {
        PlayerCharacter* Character = new PlayerCharacter();
        Character->SetInfo(object, name);
        object->AddComponent(Character);
    }
    else if (name == "GameManager")
    {
        GameManager* GameMgr = new GameManager();
        GameMgr->SetInfo(object, name);
        object->AddComponent(GameMgr);
    }
    else if (name == "PlayerGUI")
    {
        PlayerGUI* GUI = new PlayerGUI();
        GUI->SetInfo(object, name);
        object->AddComponent(GUI);
    }
    else if (name == "Monster")
    {
        Monster* monster = new Monster();
        monster->SetInfo(object, name);
        object->AddComponent(monster);
    }


    return true;
}

void SetScriptProc(BoneEditor* editor)
{
    editor->AddScript = AddScriptHandler;

    editor->AddScriptList("TestScript2");
    editor->AddScriptList("PlayerCharacter");
    editor->AddScriptList("PlayerGUI");
    editor->AddScriptList("GameManager");
    editor->AddScriptList("Monster");
}