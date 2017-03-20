#pragma once
#include <InterfaceIds.h>
#include <Script.h>
#include <Transform3D.h>
#include <Camera.h>
#include <SceneManager.h>

using namespace BONE_GRAPHICS;

class PlayerCamera : public Script {
public:
    virtual void Init();
    virtual void Update();

private:
    Camera* mainCamera;
    int mouseX;
    int mouseY;
    float cameraMoveSpeed;
};
