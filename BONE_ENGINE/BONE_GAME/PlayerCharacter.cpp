#include <StaticMesh.h>
#include <InputManager.h>

#include "PlayerCharacter.h"
#include "PlayerGUI.h"

void PlayerCharacter::Init()
{
    gameObject->SetPriority(1);
    gameObject->SetTag("Player");
}

void PlayerCharacter::Reference()
{
    cameraObject = new GameObject();
    cameraObject->SetPriority(1);
    
    Transform3D* tr = new Transform3D();
    tr->SetPosition(Vec3(0, 30, 15) + ((Transform3D*)gameObject->transform3D)->GetPosition());
    cameraObject->AddComponent(tr);
    cameraTr = tr;

    Camera *camera = new Camera(0, PROJECTION_TYPE::PRJOJECTION_PERSPACTIVE,
        Vec3(0, 1, 0), RenderMgr->GetWidth(), RenderMgr->GetHeight(), 1000, 0.1f, D3DX_PI * 0.6f);
    camera->SetTargetPosition(((Transform3D*)gameObject->transform3D)->GetPosition() + Vec3(0, 30, 0));
    cameraObject->AddComponent(camera);
    mainCamera = camera;

    skinnedMesh = ((SkinnedMesh*)gameObject->GetComponent("SkinnedMesh"));
    CUR_SCENE->AddObject(cameraObject, "MainCamera");

    GameObject* Sword = new GameObject();
    Sword->SetPriority(1);

    Transform3D* SwordTransform = new Transform3D();
    auto BoneMatrix = skinnedMesh->GetBoneMatrix("hand_r");
    SwordTransform->CombineMatrix(BoneMatrix);
    SwordTransform->SetPosition(-19.0f, 5.0f, 17.0f);
    SwordTransform->SetRotate(-0.45f, 0.96f, 1.85f);
    SwordTransform->SetScale(2.0f, 2.0f, 2.0f);
    Sword->AddComponent(SwordTransform);
    swordTr = SwordTransform;

    StaticMesh* SwordMesh = new StaticMesh();
    SwordMesh->SetFile("Sword.X");
    Sword->AddComponent(SwordMesh);

    Sword->AttachParent(this->gameObject);

    CUR_SCENE->AddObject(Sword, "Sword");

    gui = (PlayerGUI*)(CUR_SCENE->FindObjectByName("GameManager")->GetComponent("PlayerGUI"));
}

void PlayerCharacter::Update()
{
    if (isEvent)
    {
        gui->ShowGUI(false);

        GET_RIGIDBODY(gameObject)->SetLinearVelocity(0, 0, 0);
        return;
    }

    gui->ShowGUI(true);
    
    cout << swordTr->GetPosition().x << ", " << swordTr->GetPosition().y << ", " << swordTr->GetPosition().z << endl;

    bool Input = false;
        
    static bool W_Key = false;
    static bool S_Key = false;
    static bool A_Key = false;
    static bool D_Key = false;
    static bool Attack_Key = false;

    if (InputMgr->KeyDown('W', false) && !Attack_Key)
    {
        Input = true;
        skinnedMesh->SetAnimation("Skeleton_1H_walk");

        Vec3 Forward = GET_TRANSFORM_3D(gameObject)->GetForward() * SceneMgr->GetTimeDelta() * 1000;

        GET_RIGIDBODY(gameObject)->SetLinearVelocity(Forward.x, Forward.y, -Forward.z);

        W_Key = true;
    }
    else if (W_Key)
    {
        GET_RIGIDBODY(gameObject)->SetLinearVelocity(0, 0, 0);
        W_Key = false;
    }

    if (InputMgr->KeyDown('S', false) && !Attack_Key)
    {
        Input = true;
        skinnedMesh->SetAnimation("Skeleton_walking_back");

        Vec3 Backword = -GET_TRANSFORM_3D(gameObject)->GetForward() * SceneMgr->GetTimeDelta() * 1000;

        GET_RIGIDBODY(gameObject)->SetLinearVelocity(Backword.x, Backword.y, -Backword.z);

        S_Key = true;
    }
    else if (S_Key)
    {
        GET_RIGIDBODY(gameObject)->SetLinearVelocity(0, 0, 0);
        S_Key = false;
    }

    if (InputMgr->KeyDown('A', false) && !Attack_Key)
    {
        Input = true;
        A_Key = true;

        GET_RIGIDBODY(gameObject)->SetAngularVelocity(0, -1.5f, 0);
    }
    else if (A_Key == true)
    {
        GET_RIGIDBODY(gameObject)->SetAngularVelocity(0, 0, 0);
        A_Key = false;
    }
        
    if (InputMgr->KeyDown('D', false) && !Attack_Key)
    {
        Input = true;
        D_Key = true;

        GET_RIGIDBODY(gameObject)->SetAngularVelocity(0, 1.5f, 0);
    }
    else if (D_Key == true)
    {
        GET_RIGIDBODY(gameObject)->SetAngularVelocity(0, 0, 0);
        D_Key = false;
    }

    if (InputMgr->GetMouseLBButtonStatus() == MOUSE_LBDOWN)
    {
        Input = true;
        Attack_Key = true;

        skinnedMesh->SetAnimation("Skeleton_1H_swing_left");
        skinnedMesh->SetAnimationLoop(false);
    }
    else if (Attack_Key == true)
    {
        if (skinnedMesh->GetAnimationRate() >= 0.99f)
            Attack_Key = false;
        else
            Input = true;
    }

    if (Input == false)
    {
        skinnedMesh->SetAnimationLoop(true);
        skinnedMesh->SetAnimation("Skeleton_Idle");
    }
}

void PlayerCharacter::LateUpdate()
{
    if (isEvent)
        return;
     
    POINT pt;
    float fDelta = 0.001f; // 마우스의 민감도, 이 값이 커질수록 많이 움직인다.

    GetCursorPos(&pt);
    int dx = pt.x - mouseX;
    int dy = pt.y - mouseY;
                          
    Vec3 Target = GET_TRANSFORM_3D(gameObject)->GetPosition() + Vec3(0, 40, 0);
    Vec3 Eye = GET_TRANSFORM_3D(cameraObject)->GetPosition();

    Vec3 Dir = Eye - Target;

    Vec3 YRotatePos = Target;

    Matrix YMat;
    D3DXMatrixRotationY(&YMat, dx * fDelta);
    D3DXVec3TransformCoord(&YRotatePos, &Dir, &YMat);
        
    Vec3 Cross;
    D3DXVec3Normalize(&Dir, &Dir);
    D3DXVec3Cross(&Cross, &Dir, &Vec3(0, 1, 0));
    D3DXVec3Normalize(&Cross, &Cross);

    Vec3 FinalPos;

    Matrix CrossMat;
    D3DXMatrixRotationAxis(&CrossMat, &Cross, dy * fDelta);
    D3DXVec3TransformCoord(&FinalPos, &YRotatePos, &CrossMat);
    D3DXVec3Normalize(&FinalPos, &FinalPos);
    FinalPos *= 15;

    pt.x = (RenderMgr->GetWidth()) / 2;
    pt.y = (RenderMgr->GetHeight()) / 2;
                      
    SetCursorPos(pt.x, pt.y);
    mouseX = pt.x;
    mouseY = pt.y;
    
    mainCamera->SetTargetPosition(Target);
    cameraTr->SetPosition(FinalPos + Target);

    mainCamera->FixedUpdate(cameraObject);
}