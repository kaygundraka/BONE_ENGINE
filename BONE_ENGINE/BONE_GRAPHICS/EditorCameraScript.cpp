#include "Common.h"
#include "EditorCameraScript.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "StaticMesh.h"
#include "SkinnedMesh.h"
using namespace BONE_GRAPHICS;

EditorCamera::EditorCamera(GameObject* gameObject, BoneEditor* ui, std::string name)
{
    SetInfo(gameObject, name);
    this->ui = ui;
}

EditorCamera::~EditorCamera()
{

}

void EditorCamera::Init()
{
    gameObject->SetPriority(1);
    gameObject->SetTag("EditorObject");

    Transform3D* tr = new Transform3D();
    tr->SetPosition(Vec3(200, 200, 200));
    gameObject->AddComponent(tr);

    Camera *camera = new Camera(0, PROJECTION_TYPE::PRJOJECTION_PERSPACTIVE,
        Vec3(0, 1, 0), RenderMgr->GetWidth(), RenderMgr->GetHeight(), 1000, 0.1f, D3DX_PI * 0.6f);

    camera->SetTargetPosition(Vec3(0, 0, 0));
    gameObject->AddComponent(camera);

    mainCamera = ((Camera*)gameObject->GetComponent("Camera"));

    mouseX = 0;
    mouseY = 0;

    selectObject = "";

    cameraMoveSpeed = 100.0f;

    cameraMove = false;
}

void EditorCamera::Reference()
{
}

void EditorCamera::Update()
{
    if (ui->IsEditMode())
        return;

    if (InputMgr->GetMouseRBButtonStatus() == MOUSE_STATUS::MOUSE_RBDOWN)
        cameraMove = true;
    else if (InputMgr->GetMouseRBButtonStatus() == MOUSE_STATUS::MOUSE_RBUP)
        cameraMove = false;
    
    if (cameraMove)
    {
        POINT pt;
        float fDelta = 0.001f; // 마우스의 민감도, 이 값이 커질수록 많이 움직인다.

        GetCursorPos(&pt);
        int dx = pt.x - mouseX; // 마우스의 변화값
        int dy = pt.y - mouseY; // 마우스의 변화값

        mainCamera->RotateLocalX(dy * fDelta); // 마우스의 Y축 회전값은 3D world의  X축 회전값
        mainCamera->RotateLocalY(dx * fDelta); // 마우스의 X축 회전값은 3D world의  Y축 회전값

        pt.x = (RenderMgr->GetWidth()) / 2;
        pt.y = (RenderMgr->GetHeight()) / 2;
                                         
        SetCursorPos(pt.x, pt.y);
        mouseX = pt.x;
        mouseY = pt.y;
    }

    auto transform = ((Transform3D*)gameObject->transform3D);

    cameraMoveSpeed = 100.0f;

    if (InputMgr->KeyDown(VK_SHIFT, false))
        cameraMoveSpeed = 200.0f;
    
    if (InputMgr->GetMouseWheelStatus() == MOUSE_WHEEL_DOWN)
    {
        auto ray = RenderMgr->GetPickingRayToView(false);
        
        transform->SetPosition(
            transform->GetPosition() -
            ray.direction *
            SceneMgr->GetTimeDelta() * cameraMoveSpeed
        );

        mainCamera->SetTargetPosition(
            mainCamera->GetTargetPosition() -
            ray.direction *
            SceneMgr->GetTimeDelta() * cameraMoveSpeed
        );
    }

    if (InputMgr->GetMouseWheelStatus() == MOUSE_WHEEL_UP)
    {
        auto ray = RenderMgr->GetPickingRayToView(false);

        transform->SetPosition(
            transform->GetPosition() +
            ray.direction *
            SceneMgr->GetTimeDelta() * cameraMoveSpeed
        );

        mainCamera->SetTargetPosition(
            mainCamera->GetTargetPosition() +
            ray.direction *
            SceneMgr->GetTimeDelta() * cameraMoveSpeed
        );
    }

    if (InputMgr->KeyDown('W', false))
    {
        transform->SetPosition(
            transform->GetPosition() +
            mainCamera->GetViewVector() *
            SceneMgr->GetTimeDelta() * cameraMoveSpeed
        );

        mainCamera->SetTargetPosition(
            mainCamera->GetTargetPosition() +
            mainCamera->GetViewVector() *
            SceneMgr->GetTimeDelta() * cameraMoveSpeed
        );
    }

    if (InputMgr->KeyDown('S', false))
    {
        transform->SetPosition(
            transform->GetPosition() +
            -mainCamera->GetViewVector() *
            SceneMgr->GetTimeDelta() * cameraMoveSpeed
        );

        mainCamera->SetTargetPosition(
            mainCamera->GetTargetPosition() +
            mainCamera->GetViewVector() *
            SceneMgr->GetTimeDelta() * cameraMoveSpeed
        );
    }

    if (InputMgr->KeyDown('A', false))
    {
        transform->SetPosition(
            transform->GetPosition() +
            -mainCamera->GetCrossVector() *
            SceneMgr->GetTimeDelta() * cameraMoveSpeed
        );

        mainCamera->SetTargetPosition(mainCamera->GetTargetPosition() +
            -mainCamera->GetCrossVector() *
            SceneMgr->GetTimeDelta() * cameraMoveSpeed
        );
    }

    if (InputMgr->KeyDown('D', false))
    {
        transform->SetPosition(transform->GetPosition() +
            mainCamera->GetCrossVector() *
            SceneMgr->GetTimeDelta() * cameraMoveSpeed
        );

        mainCamera->SetTargetPosition(mainCamera->GetTargetPosition() +
            mainCamera->GetCrossVector() *
            SceneMgr->GetTimeDelta() * cameraMoveSpeed
        );
    }

}

void EditorCamera::LateRender()
{
    Vec3 Pos[2];

    Pos[0].y = 0;
    Pos[1].y = 0;

    for (int i = 0; i < 81; i++)
    {
        Pos[0].x = -400 + i * 10;
        Pos[1].x = -400 + i * 10;
        Pos[0].z = -400;
        Pos[1].z = 400;

        if (i == 40)
            RenderMgr->DrawLine(Pos[0], Pos[1], COLOR::RED);
        else
            RenderMgr->DrawLine(Pos[0], Pos[1], COLOR::WHITE);
    }

    for (int i = 0; i < 81; i++)
    {
        Pos[0].z = -400 + i * 10;
        Pos[1].z = -400 + i * 10;
        Pos[0].x = -400;
        Pos[1].x = 400;

        if (i == 40)
            RenderMgr->DrawLine(Pos[0], Pos[1], COLOR::BLUE);
        else
            RenderMgr->DrawLine(Pos[0], Pos[1], COLOR::WHITE);
    }
}

void EditorCamera::LateUpdate()
{
    mainCamera->FixedUpdate(gameObject);
}