#include "Common.h"
#include "PosPivot.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "Camera.h"

namespace BONE_GRAPHICS
{
    PosPivot::PosPivot(BoneEditor* ui, GameObject* gameObject, std::string name)
    {
        SetInfo(gameObject, name);
        this->ui = ui;
    }    
    
    PosPivot::~PosPivot()
    {

    }

    void PosPivot::Init()
    {
        gameObject->SetPriority(1);
        gameObject->SetTag("EditorObject");

        Transform3D* Transform = new Transform3D();
        gameObject->AddComponent(Transform);

        moveX = false;
        moveY = false;
        moveZ = false;

        moveValue = 0;
    }

    void PosPivot::Reference()
    {
        for (int i = 0; i < 3; i++) 
        {
            GameObject* Pivot = new GameObject();
            StaticMesh* PivotMesh = new StaticMesh();
            Transform3D* Transform = new Transform3D();

            Pivot->SetPipeLine(GameObject::DIRECT_DEFAULT);

            PivotMesh->SetFile("PosPivot.X");
            PivotMesh->Hide();
            
            Pivot->AddComponent(Transform);
            Pivot->AddComponent(PivotMesh);
            Pivot->SetTag("EditorObject");

            std::string Name;

            if (i == 0)
                Name = "Pivot_X";
            else if (i == 1)
            {
                Name = "Pivot_Y";
                Transform->SetRotate(0, 0, 3.14f / 2);
            }
            else
            {
                Name = "Pivot_Z";
                Transform->SetRotate(0, -3.14f / 2, 0);
            }

            SceneMgr->CurrentScene()->AddObject(Pivot, Name);

            Pivot->AttachParent(gameObject);

            std::string* Texture = new std::string;
            if (i == 0)
                *Texture = "red.png";
            else if (i == 1)
                *Texture = "green.png";
            else
                *Texture = "blue.png";


            PivotMesh->SetTextures(Texture);
        }

        mainCamera = SceneMgr->CurrentScene()->GetCurrentCamera();
    }

    void PosPivot::Update()
    {
        if (!InputMgr->IsFocusedWindow())
            return;

        if (InputMgr->KeyDown('F', true))
        {
            auto Object = SceneMgr->CurrentScene()->FindObjectByName(selectObject);

            if (Object != nullptr)
            {
                auto Position = ((Transform3D*)Object->transform3D)->GetPosition();
                ((Camera*)mainCamera->GetComponent("Camera"))->SetTargetPosition(Position);
                ((Transform3D*)mainCamera->transform3D)->SetPosition(
                    Position + Vec3(20, 20, 20)
                );
            }
        }

        if (InputMgr->GetMouseLBButtonStatus() == MOUSE_STATUS::MOUSE_LBUP)
        {
            moveX = false;
            moveY = false;
            moveZ = false;
        }

        if (ui->IsEditMode())
            return;

        if (InputMgr->GetMouseLBButtonStatus() == MOUSE_STATUS::MOUSE_LBDOWN && !moveX && !moveY && !moveZ)
        {
            auto ObjectList = SceneMgr->CurrentScene()->GetObjectList();
            auto StaticObjectList = SceneMgr->CurrentScene()->GetStaticObjectList();

            float MinDist = -1;
            bool NoSelect = true;

            for each(auto item in ObjectList)
            {
                StaticMesh* mesh = (StaticMesh*)(item->GetComponent("StaticMesh"));

                if (mesh != nullptr && selectObject != "")
                {
                    float Dist = 0;

                    if (mesh->GetFile() == "PosPivot.X")
                    {
                        if (mesh->CheckMouseRayInMesh((Transform3D*)item->transform3D, &Dist))
                        {
                            if (item->GetName() == "Pivot_X")
                                moveX = true;
                            else if (item->GetName() == "Pivot_Y")
                                moveY = true;
                            else
                                moveZ = true;

                            NoSelect = false;

                            break;
                        }
                    }
                }
            }

            for each(auto item in ObjectList)
            {
                StaticMesh* mesh = (StaticMesh*)(item->GetComponent("StaticMesh"));

                if (mesh != nullptr)
                {
                    float Dist = 0;

                    if (mesh->GetFile() == "PosPivot.X")
                        continue;

                    if (mesh->CheckMouseRayInMesh((Transform3D*)item->transform3D, &Dist))
                    {
                        if (MinDist == -1)
                        {
                            selectObject = item->GetName();
                            MinDist = Dist;
                        }
                        else if (MinDist >= Dist)
                        {
                            MinDist = Dist;
                            selectObject = item->GetName();
                        }

                        NoSelect = false;
                    }
                }
            }

            for each(auto item in StaticObjectList)
            {
                StaticMesh* mesh = (StaticMesh*)(item->GetComponent("StaticMesh"));

                if (mesh != nullptr)
                {
                    float Dist = 0;

                    if (mesh->CheckMouseRayInMesh((Transform3D*)item->transform3D, &Dist))
                    {
                        if (MinDist == -1)
                        {
                            selectObject = item->GetName();
                            MinDist = Dist;
                        }
                        else if (MinDist >= Dist)
                        {
                            MinDist = Dist;
                            selectObject = item->GetName();
                        }
                    }

                    NoSelect = false;
                }
            }

            auto Object = SceneMgr->CurrentScene()->FindObjectByName(selectObject);
            StaticMesh* sm = nullptr;

            if (Object != nullptr)
                sm = ((StaticMesh*)Object->GetComponent("StaticMesh"));

            if (NoSelect)
            {
                if (Object != nullptr)
                {
                    sm->ShowMeshBox(false);
                    sm->SetMeshBoxColor(COLOR::WHITE);
                }

                selectObject = "";
            }
            else
            {
                ui->SelectObject(selectObject);

                if (Object != nullptr)
                {
                    sm->ShowMeshBox(true);
                    sm->SetMeshBoxColor(COLOR::GREEN);
                }
            }
        }
        
        if (selectObject != "")
        {
            auto Object = SceneMgr->CurrentScene()->FindObjectByName(selectObject);

            while (Object->GetParent() != nullptr)
                Object = Object->GetParent();
            
            Vec3 ObjectPos = ((Transform3D*)Object->transform3D)->GetPosition();
            Vec3 CameraPos = ((Transform3D*)mainCamera->transform3D)->GetPosition();
            Vec3 Vector = ObjectPos - CameraPos;

            float Length = D3DXVec3Length(&Vector);

            D3DXVec3Normalize(&Vector, &Vector);

            ((Transform3D*)gameObject->transform3D)->SetPosition(CameraPos + Vector * Length / 2);
            ((Transform3D*)gameObject->transform3D)->SetScale(0.005f * Length / 2, 0.005f * Length / 2, 0.005f * Length / 2);

            if (!isShow)
            {
                isShow = true;

                for each(auto var in gameObject->GetChileds())
                    ((StaticMesh*)var->GetComponent("StaticMesh"))->Show();
            }
        }
        else if (isShow)
        {
            isShow = false;

            for each(auto var in gameObject->GetChileds())
                ((StaticMesh*)var->GetComponent("StaticMesh"))->Hide();
        }

        if ((moveX || moveY || moveZ) && selectObject != "")
        {
            GameObject* pivot = nullptr;
            
            if (moveX)
                pivot = SceneMgr->CurrentScene()->FindObjectByName("Pivot_X");
            else if (moveY)
                pivot = SceneMgr->CurrentScene()->FindObjectByName("Pivot_Y");
            else
                pivot = SceneMgr->CurrentScene()->FindObjectByName("Pivot_Z");
            
            StaticMesh* mesh = (StaticMesh*)(pivot->GetComponent("StaticMesh"));

            float Dist;

            if (mesh->CheckMouseRayInMesh((Transform3D*)pivot->transform3D, &Dist))
                moveValue = Dist;

            auto Object = SceneMgr->CurrentScene()->FindObjectByName(selectObject);

            if (Object->IsLockedEditor())
                return;
/*
            while (Object->GetParent() != nullptr)
                Object = Object->GetParent();
*/
            RAY PickingRay = RenderMgr->GetPickingRayToView(false);
            Vec3 PickingPos = PickingRay.origin + PickingRay.direction * moveValue * 2;

            Vec3 MovePos = ((Transform3D*)Object->transform3D)->GetPosition();

            if (moveValue != 0)
            {
                if (moveX)
                    MovePos.x = PickingPos.x;
                else if (moveY)
                    MovePos.y = PickingPos.y;
                else
                    MovePos.z = PickingPos.z;
            }

            ((Transform3D*)Object->transform3D)->SetPosition(MovePos);
        }
    }
}