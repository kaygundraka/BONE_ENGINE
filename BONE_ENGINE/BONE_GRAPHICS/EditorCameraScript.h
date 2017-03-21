#pragma once
#include "Camera.h"
#include "Script.h"
#include "BoneEditor.h"
using namespace BONE_GRAPHICS;

class EditorCamera : public Script {
private:
    Camera* mainCamera;
    int mouseX;
    int mouseY;
    float cameraMoveSpeed;

    std::string selectObject;

    bool cameraMove;
    BoneEditor* ui;

public:
    EditorCamera(GameObject* gameObject, BoneEditor* ui, std::string name);
    virtual ~EditorCamera();

    virtual void Init();
    virtual void Reference();
    virtual void Update();
    virtual void LateRender();
    virtual void LateUpdate();
};