#pragma once
#include <Camera.h>
#include <GameObject.h>
#include <Transform3D.h>
using namespace BONE_GRAPHICS;

class CameraObject : public GameObject {
private:
    Camera* mainCamera;
    int mouseX;
    int mouseY;

    std::string selectObject;

    bool cameraMove;

public:
    virtual void Init();
    virtual void Update();
    virtual void LateUpdate();
};