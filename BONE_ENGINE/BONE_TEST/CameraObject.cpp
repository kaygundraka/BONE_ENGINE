#include "CameraObject.h"

void CameraObject::Init()
{
    SetPriority(1);
    SetTag("CameraObject");

    Transform3D* tr = new Transform3D();
    tr->SetPosition(Vector3(0, 200, 200));
    this->AddComponent(tr);
    transform = ((Transform3D*)GetComponent("Transform3D"));

    Camera *camera = new Camera(0, PROJECTION_TYPE::PRJOJECTION_PERSPACTIVE,
        Vector3(0, 1, 0), RenderMgr->GetWidth(), RenderMgr->GetHeight(), 1000, 0.1f, D3DX_PI * 0.3f);

    camera->SetTargetPosition(Vector3(0, 0, 0));
    this->AddComponent(camera);

    mainCamera = ((Camera*)GetComponent("Camera"));
}

void CameraObject::LateUpdate()
{
    mainCamera->FixedUpdate(this);
}