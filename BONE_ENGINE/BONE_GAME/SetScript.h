#pragma once
#include <iostream>
#include <GameObject.h>
#include <RuntimeCompiler.h>
#include <Script.h>
#include <BoneEditor.h>
using namespace BONE_GRAPHICS;

namespace BONE_GRAPHICS {
    void Init(GameObject* owner);

    void Awake(GameObject* owner);

    void Reference(GameObject* owner);

    void Update(GameObject* owner);

    void LateUpdate(GameObject* owner);

    void LateRender(GameObject* owner);

    void CollisionEvent(GameObject* owner, GameObject* otherObject);

    void SetGameObject(GameObject* obj);
}

bool AddScriptHandler(GameObject* object, std::string name);

void SetScriptProc(BoneEditor* editor);