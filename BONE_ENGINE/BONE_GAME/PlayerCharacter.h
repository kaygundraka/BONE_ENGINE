#pragma once
#include <InterfaceIds.h>
#include <Script.h>
#include <Transform3D.h>
#include <Camera.h>
#include <InputManager.h>
#include <SceneManager.h>
#include <RenderManager.h>
#include <StaticMesh.h>
#include <SkinnedMesh.h>
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

    float speed;

    bool W_Key;
    bool S_Key;
    bool A_Key;
    bool D_Key;

    bool isSneaking;
    bool isRun;

    bool Sneaking_Key;
    bool Attack_Key;

public:
    bool isEvent;
    
    virtual void Init();
    virtual void Reference();
    virtual void Update();
    virtual void LateUpdate();
};
