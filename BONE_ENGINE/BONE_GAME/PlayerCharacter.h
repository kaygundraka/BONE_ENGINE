#pragma once
#include <InterfaceIds.h>
#include <Script.h>
#include <Transform3D.h>
#include <Camera.h>
#include <InputManager.h>
#include <SceneManager.h>
#include <RenderManager.h>
#include <SoundManager.h>
#include <StaticMesh.h>
#include <SkinnedMesh.h>
#include <Rp3dRigidBody.h>
#include <SoundClip.h>
#include "PlayerGUI.h"

using namespace BONE_GRAPHICS;

class PlayerCharacter : public Script {
private:
    GameObject* cameraObject;
    Camera* mainCamera;
    Transform3D* cameraTr;
    
    GameObject* sword;
    Transform3D* swordTr;

    GameObject* shield;
    Transform3D* shieldTr;

    SoundClip* soundClips;


    BONE_GRAPHICS::RigidBody* rigidBody;
    Transform3D* transform;
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
    bool isCombat;
    bool wearItem;

    bool Sneaking_Key;
    bool Attack_Key;

public:
    bool isEvent;

    void CombatMode();
    void NormalMode();
    void WearItem();
    bool IsWoreItem();
    
    virtual void Init();
    virtual void Reference();
    virtual void Update();
    virtual void LateUpdate();
};
