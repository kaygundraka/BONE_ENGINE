#include "GameManager.h"
#include "PlayerCharacter.h"
#include <InputManager.h>
#include <SceneManager.h>

void GameManager::Init()
{
    screenAlpha = 255;
    firstSceneFlow = 0;
    totalSceneFlow = 0;
    subtitleSize = 0;

    if (!CUR_SCENE->IsEditorScene())
    {
        screenImage = new ScreenImage();
        screenImage->SetImageFile("black.png");
        screenImage->SetAlpha(screenAlpha);
        screenImage->SetOriginRect(Vec2(0, 0), Vec2(RenderMgr->GetWidth(), RenderMgr->GetHeight()));

        gameObject->AddComponent(screenImage);

        Transform2D* tr2d = new Transform2D();
        tr2d->SetPosition(Vec3(0, 0, 0));
        gameObject->AddComponent(tr2d);
    }
}

void GameManager::Reference()
{
    firstSceneCamera = new GameObject();
    firstSceneCamera->SetPriority(1);

    Transform3D* tr = new Transform3D();
    tr->SetPosition(Vec3(-142, 50, 0) + ((Transform3D*)gameObject->transform3D)->GetPosition());
    firstSceneCamera->AddComponent(tr);
    
    Camera *camera = new Camera(1, PROJECTION_TYPE::PRJOJECTION_PERSPACTIVE,
        Vec3(0, 1, 0), RenderMgr->GetWidth(), RenderMgr->GetHeight(), 1000, 0.1f, D3DX_PI * 0.6f);
    camera->SetTargetPosition(Vec3(-162, 5, 69));
    firstSceneCamera->AddComponent(camera);

    FSCameraComponent = camera;

    player = CUR_SCENE->FindObjectByTag("Player");

    SceneMgr->CurrentScene()->AddObject(firstSceneCamera, "FirstSceneCamera");
}

void GameManager::Update()
{
    if (totalSceneFlow == 0)
    {
        ((PlayerCharacter*)player->GetComponent("PlayerCharacter"))->isEvent = true;
        CUR_SCENE->SetCameraIndex(1);

        if (firstSceneFlow == 0)
        {
            GET_SKINNED_MESH(player)->SetAnimation("Skeleton_sleeping");

            if (InputMgr->KeyDown(VK_SPACE, true))
            {
                subtitleSize = 0;
                firstSceneFlow++;
            }
        }
        else if (firstSceneFlow < 6)
        {
            FadeIn();

            if (InputMgr->KeyDown(VK_SPACE, true) && screenAlpha == 0)
            {
                subtitleSize = 0;
                firstSceneFlow++;
            }
        }
        else if (firstSceneFlow == 6)
        {
            GET_SKINNED_MESH(player)->SetAnimationLoop(false);
            GET_SKINNED_MESH(player)->SetAnimation("Skeleton_waking_up");
            
            if (GET_SKINNED_MESH(player)->GetAnimationRate() >= 0.99f)
                firstSceneFlow++;
        }
        else if (firstSceneFlow == 7)
        {
            GET_SKINNED_MESH(player)->SetAnimation("Skeleton_Looking_around");

            if (GET_SKINNED_MESH(player)->GetAnimationRate() >= 0.99f)
                firstSceneFlow++;
        }
        else if (firstSceneFlow == 8)
        {
            GET_SKINNED_MESH(player)->SetAnimationLoop(true);
            GET_SKINNED_MESH(player)->SetAnimation("Skeleton_Idle");

            FadeOut();

            if (screenAlpha == 255)
                totalSceneFlow = 1;
        }
    }
    else if (totalSceneFlow == 1)
    {
        SceneMgr->CurrentScene()->SetCameraIndex(0);
        ((PlayerCharacter*)player->GetComponent("PlayerCharacter"))->isEvent = false;

        FadeIn();

        if (screenAlpha == 0)
            totalSceneFlow++;
    }

    if (InputMgr->KeyDown(VK_ESCAPE, true))
    {
        CUR_SCENE->SetSceneFlag(true);
        SceneMgr->EndScene(CUR_SCENE->GetName());
    }
}

void GameManager::FadeIn()
{
    if (!CUR_SCENE->IsEditorScene())
    {
        if (screenAlpha > 0)
            screenAlpha -= SceneMgr->GetTimeDelta() * 100;
        
        if (screenAlpha <= 0)
            screenAlpha = 0;

        screenImage->SetAlpha(screenAlpha);
    }
}

void GameManager::FadeOut()
{
    if (!CUR_SCENE->IsEditorScene())
    {
        if (screenAlpha < 255)
            screenAlpha += SceneMgr->GetTimeDelta() * 100;
        else
            screenAlpha = 255;

        screenImage->SetAlpha(screenAlpha);
    }
}

void GameManager::LateUpdate()
{
    if (totalSceneFlow == 0)
        FSCameraComponent->FixedUpdate(firstSceneCamera);
}

void GameManager::LateRender()
{
    if (totalSceneFlow == 0)
    {
        if (firstSceneFlow < 6)
        {
            std::string text = "Am I alive? Is it dead?";

            if (firstSceneFlow == 1)
                text = "3 days ago, I was able to think.";
            else if (firstSceneFlow == 2)
                text = "On the second day, I was able to see.";
            else if (firstSceneFlow == 3)
                text = "On the third day, I was able to hear.";
            else if (firstSceneFlow == 4)
                text = "And now I am able to move weakly.";

            static float UpdateSubtitle = 0;

            if (UpdateSubtitle < 0.02f)
                UpdateSubtitle += SceneMgr->GetTimeDelta();
            else
            {
                UpdateSubtitle = 0;

                if (subtitleSize < text.size())
                    subtitleSize++;
            }

            RenderMgr->RenderText(text.substr(0, subtitleSize), Vec2(50, RenderMgr->GetHeight() - 100), 50, "Northwood High", 0, FW_BOLD, COLOR::WHITE);
        }
    }
}