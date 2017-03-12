#pragma once
#include <Camera.h>
#include <GameObject.h>
#include <Transform3D.h>
using namespace BONE_GRAPHICS;

class CameraObject : public GameObject {
private:
    Transform3D* transform;
    Camera* mainCamera;

public:
    virtual void Init();
    virtual void LateUpdate();
};