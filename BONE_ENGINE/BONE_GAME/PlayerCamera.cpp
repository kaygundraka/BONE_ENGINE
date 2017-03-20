#include "PlayerCamera.h"
#include <InputManager.h>

void PlayerCamera::Init()
{
    gameObject->SetPriority(1);
    
    Camera *camera = new Camera(0, PROJECTION_TYPE::PRJOJECTION_PERSPACTIVE,
        Vec3(0, 1, 0), RenderMgr->GetWidth(), RenderMgr->GetHeight(), 1000, 0.1f, D3DX_PI * 0.6f);

    camera->SetTargetPosition(Vec3(0, 0, 0));
    gameObject->AddComponent(camera);

    mainCamera = ((Camera*)gameObject->GetComponent("Camera"));

    mouseX = 0;
    mouseY = 0;

    cameraMoveSpeed = 100.0f;
}

void PlayerCamera::Update()
{
    {
        POINT pt;
        float fDelta = 0.001f;

        GetCursorPos(&pt);
        int dx = pt.x - mouseX;
        int dy = pt.y - mouseY;

        mainCamera->RotateLocalX(dy * fDelta);
        mainCamera->RotateLocalY(dx * fDelta);

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

        mainCamera->SetTargetPosition(
            mainCamera->GetTargetPosition() +
            -mainCamera->GetCrossVector() *
            SceneMgr->GetTimeDelta() * cameraMoveSpeed
        );
    }

    if (InputMgr->KeyDown('D', false))
    {
        transform->SetPosition(
            transform->GetPosition() +
            mainCamera->GetCrossVector() *
            SceneMgr->GetTimeDelta() * cameraMoveSpeed
        );

        mainCamera->SetTargetPosition(
            mainCamera->GetTargetPosition() +
            mainCamera->GetCrossVector() *
            SceneMgr->GetTimeDelta() * cameraMoveSpeed
        );
    }
}