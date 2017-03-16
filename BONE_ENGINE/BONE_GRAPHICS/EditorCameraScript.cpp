#include "Common.h"
#include "EditorCameraScript.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "StaticMesh.h"
using namespace BONE_GRAPHICS;

EditorCamera::EditorCamera(GameObject* gameObject, std::string name) : Script(gameObject, name)
{

}

EditorCamera::~EditorCamera()
{

}

void EditorCamera::Init()
{
    gameObject->SetPriority(1);
    gameObject->SetTag("EditorObject");

    Transform3D* tr = new Transform3D();
    tr->SetPosition(Vector3(0, 100, 100));
    gameObject->AddComponent(tr);

    Camera *camera = new Camera(0, PROJECTION_TYPE::PRJOJECTION_PERSPACTIVE,
        Vector3(0, 1, 0), RenderMgr->GetWidth(), RenderMgr->GetHeight(), 1000, 0.1f, D3DX_PI * 0.3f);

    camera->SetTargetPosition(Vector3(0, 0, 0));
    gameObject->AddComponent(camera);

    mainCamera = ((Camera*)gameObject->GetComponent("Camera"));

    mouseX = 0;
    mouseY = 0;

    selectObject = "";

    cameraMove = false;

    gameObject->SetTag("EditorObject");
}

void EditorCamera::Reference()
{
}

void EditorCamera::Update()
{
    if (InputMgr->GetMouseRBButtonStatus() == MOUSE_STATUS::MOUSE_RBDOWN)
        cameraMove = true;
    else if (InputMgr->GetMouseRBButtonStatus() == MOUSE_STATUS::MOUSE_RBUP)
        cameraMove = false;
    
    if (cameraMove)
    {
        POINT pt;
        float fDelta = 0.001f; // ���콺�� �ΰ���, �� ���� Ŀ������ ���� �����δ�.

        GetCursorPos(&pt);
        int dx = pt.x - mouseX; // ���콺�� ��ȭ��
        int dy = pt.y - mouseY; // ���콺�� ��ȭ��

        mainCamera->RotateLocalX(dy * fDelta); // ���콺�� Y�� ȸ������ 3D world��  X�� ȸ����
        mainCamera->RotateLocalY(dx * fDelta); // ���콺�� X�� ȸ������ 3D world��  Y�� ȸ����

                                               //RECT rc;
                                               //GetClientRect(g_hwnd, &rc);
        pt.x = (RenderMgr->GetWidth()) / 2;
        pt.y = (RenderMgr->GetHeight()) / 2;//(rc.bottom - rc.top) / 2;
                                            //ClientToScreen(g_hwnd, &pt);
        SetCursorPos(pt.x, pt.y);
        mouseX = pt.x;
        mouseY = pt.y;
    }

    auto transform = ((Transform3D*)gameObject->transform3D);

    if (InputMgr->KeyDown('W', false))
    {
        transform->SetPosition(
            transform->GetPosition() +
            mainCamera->GetViewVector() *
            SceneMgr->GetTimeDelta() * 100
        );

        mainCamera->SetTargetPosition(
            mainCamera->GetTargetPosition() +
            mainCamera->GetViewVector() *
            SceneMgr->GetTimeDelta() * 100
        );
    }

    if (InputMgr->KeyDown('S', false))
    {
        transform->SetPosition(
            transform->GetPosition() +
            -mainCamera->GetViewVector() *
            SceneMgr->GetTimeDelta() * 100
        );

        mainCamera->SetTargetPosition(
            mainCamera->GetTargetPosition() +
            mainCamera->GetViewVector() *
            SceneMgr->GetTimeDelta() * 100
        );
    }

    if (InputMgr->KeyDown('A', false))
    {
        transform->SetPosition(
            transform->GetPosition() +
            -mainCamera->GetCrossVector() *
            SceneMgr->GetTimeDelta() * 100
        );

        mainCamera->SetTargetPosition(
            mainCamera->GetTargetPosition() +
            -mainCamera->GetCrossVector() *
            SceneMgr->GetTimeDelta() * 100
        );
    }

    if (InputMgr->KeyDown('D', false))
    {
        transform->SetPosition(
            transform->GetPosition() +
            mainCamera->GetCrossVector() *
            SceneMgr->GetTimeDelta() * 100
        );

        mainCamera->SetTargetPosition(
            mainCamera->GetTargetPosition() +
            mainCamera->GetCrossVector() *
            SceneMgr->GetTimeDelta() * 100
        );
    }

}

void EditorCamera::LateRender()
{
    Vector3 Pos[2];

    Pos[0].y = 0;
    Pos[1].y = 0;

    for (int i = 0; i < 9; i++)
    {
        Pos[0].x = -40 + i * 10;
        Pos[1].x = -40 + i * 10;
        Pos[0].z = -40;
        Pos[1].z = 40;

        if (i == 4)
            RenderMgr->DrawLine(Pos[0], Pos[1], COLOR::RED);
        else
            RenderMgr->DrawLine(Pos[0], Pos[1], COLOR::WHITE);
    }

    for (int i = 0; i < 9; i++)
    {
        Pos[0].z = -40 + i * 10;
        Pos[1].z = -40 + i * 10;
        Pos[0].x = -40;
        Pos[1].x = 40;

        if (i == 4)
            RenderMgr->DrawLine(Pos[0], Pos[1], COLOR::BLUE);
        else
            RenderMgr->DrawLine(Pos[0], Pos[1], COLOR::WHITE);
    }
}

void EditorCamera::LateUpdate()
{
    mainCamera->FixedUpdate(gameObject);
}