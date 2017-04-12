#pragma once
#include <InterfaceIds.h>
#include <Script.h>
#include <Transform3D.h>
#include <Camera.h>
#include <SkinnedMesh.h>
#include <SceneManager.h>
#include <Rp3dRigidBody.h>
#include "PlayerGUI.h"

using namespace BONE_GRAPHICS;

class PlayerCharacter : public Script {
private:
    Camera* mainCamera;
    
    GameObject* cameraObject;
    
    Transform3D* cameraTr;
    Transform3D* swordTr;
    
    SkinnedMesh* skinnedMesh;
    
    PlayerGUI* gui;

    float mouseX;
    float mouseY;

public:
    bool isEvent;
    
    virtual void Init();
    virtual void Reference();
    virtual void Update();
    virtual void LateUpdate();
};
