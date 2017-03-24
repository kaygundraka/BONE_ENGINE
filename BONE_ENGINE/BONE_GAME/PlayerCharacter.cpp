#include "PlayerCharacter.h"
#include <InputManager.h>
//
////Returns the Yaw, Pitch, and Roll components of this matrix. This
////function only works with pure rotation matrices.
////yaw=v[0], pitch=v[1], roll=v[2]
//void GetRotation(Matrix mat, float *v)
//{
//    //yaw=v[0], pitch=v[1], roll=v[2]
//    //Note, we use the cosf function rather than sinf just in case the
//    //angles are greater than [-1,+1]
//    v[1] = -asinf(mat._32); //pitch
//    float cp = cosf(v[1]);
//
//    //_22 = cr * cp;
//    float cr = mat._22 / cp;
//    v[2] = acosf(cr);
//
//    //_33 = cy * cp;
//    float cy = mat._33 / cp;
//    v[0] = acosf(cy);
//}
//
////creates a rotation matrix based on euler angles Y * P * R
////in the same order as DirectX.
//void Rotate(Matrix mat, const float *v)
//{
//    //yaw=v[0], pitch=v[1], roll=v[2]
//    float cy = cosf(v[0]);
//    float cp = cosf(v[1]);
//    float cr = cosf(v[2]);
//    float sp = sinf(v[1]);
//    float sr = sinf(v[2]);
//    float sy = sinf(v[0]);
//
//    mat._11 = cy * cr + sr * sp * sy;
//    mat._12 = sr * cp;
//    mat._13 = cr * -sy + sr * sp * cy;
//    mat._21 = -sr * cy + cr * sp * sy;
//    mat._22 = cr * cp;
//    mat._23 = -sr * -sy + cr * sp * cy;
//    mat._31 = cp * sy;
//    mat._32 = -sp;
//    mat._33 = cy * cp;
//}

void PlayerCharacter::Init()
{
    gameObject->SetPriority(1);
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
    SceneMgr->CurrentScene()->AddObject(cameraObject, "MainCamera");
}

void PlayerCharacter::Update()
{
    bool Input = false;

    if (InputMgr->KeyDown(VK_ESCAPE, true))
    {
        SceneMgr->CurrentScene()->SetSceneFlag(true);
        SceneMgr->EndScene(SceneMgr->CurrentScene()->GetName());
    }

    if (InputMgr->KeyDown('W', false))
    {
        Input = true;
        skinnedMesh->SetAnimation("Skeleton_1H_walk");
    }

    if (InputMgr->KeyDown('A', false))
    {
        Input = true;
        ((Transform3D*)gameObject->transform3D)->Rotate(0, 1.0f * SceneMgr->GetTimeDelta(), 0.0f);
    }

    if (Input == false)
        skinnedMesh->SetAnimation("Skeleton_Idle");
}

void PlayerCharacter::LateUpdate()
{
    POINT pt;
    float fDelta = 0.001f; // 마우스의 민감도, 이 값이 커질수록 많이 움직인다.

    GetCursorPos(&pt);
    int dx = pt.x - mouseX;
    int dy = pt.y - mouseY;
                          
    Vec3 Target = ((Transform3D*)gameObject->transform3D)->GetPosition() + Vec3(0, 30, 0);
    Vec3 Eye = ((Transform3D*)cameraObject->transform3D)->GetPosition();

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

    ((Transform3D*)cameraObject->transform3D)->SetPosition(FinalPos + Target);
    pt.x = (RenderMgr->GetWidth()) / 2;
    pt.y = (RenderMgr->GetHeight()) / 2;
                      
    SetCursorPos(pt.x, pt.y);
    mouseX = pt.x;
    mouseY = pt.y;
}