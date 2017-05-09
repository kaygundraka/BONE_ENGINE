#include "GameManager.h"
#include "PlayerCharacter.h"
#include <InputManager.h>
#include <SceneManager.h>
#include <SoundManager.h>
#include "Monster.h"

void GameManager::Init()
{
    gameObject->SetPriority(2);

    screenAlpha = 255;
    sceneFlow[0] = 0;
    sceneFlow[1] = 0;
    sceneFlow[2] = 0;
    totalSceneFlow = 0;
    inDoorRange = false;
    inKeysRange = false;
    subtitleSize = 0;
    hasKey = false;

    if (!CUR_SCENE->IsEditorScene())
    {
        screenImage = new ScreenImage();
        screenImage->SetImageFile("black.png");
        screenImage->SetAlpha(screenAlpha);
        screenImage->SetOriginRect(Vec2(0, 0), Vec2(RenderMgr->GetWidth(), RenderMgr->GetHeight()));

        gameObject->AddComponent(screenImage);

        Transform2D* tr2d = new Transform2D();
        tr2d->SetPosition(Vec3(0, 0, 2));
        gameObject->AddComponent(tr2d);
    }
}

void GameManager::Reference()
{
    SoundMgr->Play2D("Quiet.mp3", 0.3f, true);

    player = CUR_SCENE->FindObjectByTag("Player");

#pragma region Camera SetUp
    // 1_1
    sceneCamera_1_1 = new GameObject();
    sceneCamera_1_1->SetPriority(1);

    Transform3D* tr = new Transform3D();
    tr->SetPosition(Vec3(-142, 50, 0) + ((Transform3D*)gameObject->transform3D)->GetPosition());
    sceneCamera_1_1->AddComponent(tr);
    
    Camera *camera = new Camera(1, PROJECTION_TYPE::PRJOJECTION_PERSPACTIVE,
        Vec3(0, 1, 0), RenderMgr->GetWidth(), RenderMgr->GetHeight(), 1000, 0.1f, D3DX_PI * 0.6f);
    camera->SetTargetPosition(Vec3(-162, 5, 69));
    sceneCamera_1_1->AddComponent(camera);

    FSCameraComponent_1_1 = camera;

    SceneMgr->CurrentScene()->AddObject(sceneCamera_1_1, "sceneCamera_1_1");

    // 1_2 
    sceneCamera_1_2 = new GameObject();
    sceneCamera_1_2->SetPriority(1);

    Transform3D* tr2 = new Transform3D();
    tr2->SetPosition(Vec3(-150, 50, 0) + ((Transform3D*)gameObject->transform3D)->GetPosition());
    sceneCamera_1_2->AddComponent(tr2);

    Camera *camera2 = new Camera(2, PROJECTION_TYPE::PRJOJECTION_PERSPACTIVE,
        Vec3(0, 1, 0), RenderMgr->GetWidth(), RenderMgr->GetHeight(), 1000, 0.1f, D3DX_PI * 0.6f);
    camera2->SetTargetPosition(Vec3(-150, 10, -70));
    sceneCamera_1_2->AddComponent(camera2);

    FSCameraComponent_1_2 = camera2;

    SceneMgr->CurrentScene()->AddObject(sceneCamera_1_2, "sceneCamera_1_2");

    // 1_3
    sceneCamera_1_3 = new GameObject();
    sceneCamera_1_3->SetPriority(1);

    Transform3D* tr3 = new Transform3D();
    tr3->SetPosition(Vec3(-150, 50, 0) + ((Transform3D*)gameObject->transform3D)->GetPosition());
    sceneCamera_1_3->AddComponent(tr3);

    Camera *camera3 = new Camera(3, PROJECTION_TYPE::PRJOJECTION_PERSPACTIVE,
        Vec3(0, 1, 0), RenderMgr->GetWidth(), RenderMgr->GetHeight(), 1000, 0.1f, D3DX_PI * 0.6f);
    camera3->SetTargetPosition(Vec3(-150, 10, -70));
    sceneCamera_1_3->AddComponent(camera3);

    FSCameraComponent_1_3 = camera3;

    SceneMgr->CurrentScene()->AddObject(sceneCamera_1_3, "sceneCamera_1_3");

    // 1_4
    sceneCamera_1_4 = new GameObject();
    sceneCamera_1_4->SetPriority(1);

    Transform3D* tr4 = new Transform3D();
    tr4->SetPosition(Vec3(-150, 50, 0) + ((Transform3D*)gameObject->transform3D)->GetPosition());
    sceneCamera_1_4->AddComponent(tr4);

    Camera *camera4 = new Camera(4, PROJECTION_TYPE::PRJOJECTION_PERSPACTIVE,
        Vec3(0, 1, 0), RenderMgr->GetWidth(), RenderMgr->GetHeight(), 1000, 0.1f, D3DX_PI * 0.6f);
    camera4->SetTargetPosition(Vec3(-150, 10, -70));
    sceneCamera_1_4->AddComponent(camera4);

    FSCameraComponent_1_4 = camera4;

    SceneMgr->CurrentScene()->AddObject(sceneCamera_1_4, "sceneCamera_1_4");
#pragma endregion
}

void GameManager::Update()
{
    static GameObject* FirstRoomNpc;

    if (totalSceneFlow == 0)
    {        
        ((PlayerCharacter*)player->GetComponent("PlayerCharacter"))->isEvent = true;
        CUR_SCENE->SetCameraIndex(1);

        if (sceneFlow[0] == 0)
        {
            FirstRoomNpc = SceneMgr->CurrentScene()->FindObjectByName("FirstRoomNpc");
            GET_SKINNED_MESH(FirstRoomNpc)->SetAnimation("Skeleton_sitting");

            GET_SKINNED_MESH(player)->SetAnimation("Skeleton_sleeping");

            if (InputMgr->KeyDown(VK_SPACE, true))
            {
                CUR_SCENE->SortPriorityObject();
                subtitleSize = 0;
                sceneFlow[0]++;
            }
        }
        else if (sceneFlow[0] < 6)
        {
            FadeIn();

            if (InputMgr->KeyDown(VK_SPACE, true) && screenAlpha == 0)
            {
                subtitleSize = 0;
                sceneFlow[0]++;
            }
        }
        else if (sceneFlow[0] == 6)
        {
            GET_SKINNED_MESH(player)->SetAnimationLoop(false);
            GET_SKINNED_MESH(player)->SetAnimation("Skeleton_waking_up");
            
            if (GET_SKINNED_MESH(player)->GetAnimationRate() >= 0.99f)
                sceneFlow[0]++;
        }
        else if (sceneFlow[0] == 7)
        {
            GET_SKINNED_MESH(player)->SetAnimation("Skeleton_Looking_around");

            if (GET_SKINNED_MESH(player)->GetAnimationRate() >= 0.99f)
                sceneFlow[0]++;
        }
        else if (sceneFlow[0] == 8)
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
        SceneMgr->CurrentScene()->EnablePhysics(true);

        ((PlayerCharacter*)player->GetComponent("PlayerCharacter"))->isEvent = false;

        FadeIn();

        if (screenAlpha == 0)
            totalSceneFlow++;
    }
    else if (totalSceneFlow == 2)
    {
        auto Position = GET_TRANSFORM_3D(player)->GetPosition();
        
        if (Position.x >= -160 && Position.x <= -130 && sceneFlow[1] == 0)
        {
            if (Position.z >= -15 && Position.z <= 15)
            {
                FadeOut();

                ((PlayerCharacter*)player->GetComponent("PlayerCharacter"))->isEvent = true;

                if (screenAlpha == 255)
                {
                    SceneMgr->CurrentScene()->SetCameraIndex(2);
                    sceneFlow[1]++;
                }
            }
        }
        
        if (sceneFlow[1] == 1)
        {
            FadeIn();

            if (screenAlpha == 0)
                sceneFlow[1]++;
        }
        else if (sceneFlow[1] == 2)
        {
            GET_SKINNED_MESH(FirstRoomNpc)->SetAnimation("Skeleton_came_up");
            GET_SKINNED_MESH(FirstRoomNpc)->SetAnimationLoop(false);

            if (GET_SKINNED_MESH(FirstRoomNpc)->GetAnimationRate() >= 0.99f)
            {
                GET_SKINNED_MESH(FirstRoomNpc)->SetAnimation("Skeleton_pointing_A");
                GET_SKINNED_MESH(FirstRoomNpc)->SetAnimationLoop(true);

                sceneFlow[1]++;
            }
        }
        else if (sceneFlow[1] > 2 && sceneFlow[1] < 7)
        {
            if (InputMgr->KeyDown(VK_SPACE, true))
            {
                subtitleSize = 0;
                sceneFlow[1]++;
            }

            if (sceneFlow[1] == 4)
                GET_SKINNED_MESH(FirstRoomNpc)->SetAnimation("Skeleton_Talking");

            if (sceneFlow[1] == 6)
            {
                GET_SKINNED_MESH(FirstRoomNpc)->SetAnimation("Skeleton_Looking_around");

                FadeOut();

                if (screenAlpha == 255)
                    sceneFlow[1]++;
            }
        }
        else if (sceneFlow[1] == 7)
        {
            ((PlayerCharacter*)player->GetComponent("PlayerCharacter"))->isEvent = false;
            SceneMgr->CurrentScene()->SetCameraIndex(0);

            FadeIn();

            if (screenAlpha == 0)
                sceneFlow[1]++;
        }
        else if(sceneFlow[1] == 8)
            totalSceneFlow++;
    }
    else if (totalSceneFlow == 3)
    {
        auto Position = GET_TRANSFORM_3D(player)->GetPosition();

        if (Position.x >= -50 && Position.x <= 0 && sceneFlow[2] == 0)
        {
            if (Position.z >= -30 && Position.z <= 30)
            {
                FadeOut();

                ((PlayerCharacter*)player->GetComponent("PlayerCharacter"))->isEvent = true;

                if (screenAlpha == 255)
                {
                    SceneMgr->CurrentScene()->SetCameraIndex(3);
                    sceneFlow[2]++;
                }
            }
        }
        else if (sceneFlow[2] == 1)
        {
            FadeIn();

            if (screenAlpha == 0)
                sceneFlow[2]++;
        }
        else if (sceneFlow[2] == 2)
        {
            if (InputMgr->KeyDown(VK_SPACE, true))
            {
                subtitleSize = 0;
                sceneFlow[2]++;
            }
        }
        else if (sceneFlow[2] == 3)
        {
            FadeOut();

            if (screenAlpha == 255)
                sceneFlow[2]++;
        }
        else if (sceneFlow[2] == 4)
        {
            ((PlayerCharacter*)player->GetComponent("PlayerCharacter"))->isEvent = false;
            SceneMgr->CurrentScene()->SetCameraIndex(0);

            FadeIn();

            if (screenAlpha == 0)
                sceneFlow[2]++;
        }
        else if (sceneFlow[2] == 5)
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
            screenAlpha -= SceneMgr->GetTimeDelta() * 150;
        
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
            screenAlpha += SceneMgr->GetTimeDelta() * 150;
        
        if (screenAlpha >= 255)
            screenAlpha = 255;

        screenImage->SetAlpha(screenAlpha);
    }
}

void GameManager::LateUpdate()
{
    if (totalSceneFlow == 0)
        FSCameraComponent_1_1->FixedUpdate(sceneCamera_1_1);
    else if (totalSceneFlow == 2)
        FSCameraComponent_1_2->FixedUpdate(sceneCamera_1_2);
    else if (totalSceneFlow == 3)
    {
        auto Monsters = CUR_SCENE->FindObjectsByTag("Monster");
        auto MonsterPosition = GET_TRANSFORM_3D((Monster*)(std::get<0>(Monsters)[0]))->GetPosition();

        GET_TRANSFORM_3D(sceneCamera_1_3)->SetPosition(MonsterPosition + Vec3(0, 30, 30));

        FSCameraComponent_1_3->SetTargetPosition(MonsterPosition + Vec3(0, 20, 0));
        FSCameraComponent_1_3->FixedUpdate(sceneCamera_1_3);

        delete[] std::get<0>(Monsters);
    }
}

void GameManager::LateRender()
{
    if (totalSceneFlow == 0)
    {
        if (sceneFlow[0] < 6)
        {
            std::string text = "Am I alive? ... or dead?";

            if (sceneFlow[0] == 1)
                text = "The very next day, I was able to think.";
            else if (sceneFlow[0] == 2)
                text = "On the second day, I was able to hear.";
            else if (sceneFlow[0] == 3)
                text = "On the third day, My eyes became clear.";
            else if (sceneFlow[0] == 4)
                text = "And now I am able to move weakly.";

            ShowSubtitle(text);
        }
    }
    else if (totalSceneFlow == 2)
    {
        if (sceneFlow[1] > 2 && sceneFlow[1] < 6)
        {
            std::string text = "You are... awake.";

            if (sceneFlow[1] == 4)
                text = "Get out of here quickly.";
            else if (sceneFlow[1] == 5)
                text = "I don't have time to explain in detail.";

            ShowSubtitle(text);
        }
    }
    else if (totalSceneFlow == 3)
    {
        if (sceneFlow[2] == 2)
        {
            std::string text = "The enemy is patrolling.";

            ShowSubtitle(text);
        }
    }
    else if (totalSceneFlow == 4)
    {
        auto Position = GET_TRANSFORM_3D(player)->GetPosition();
 
        if (Position.x >= 228 && Position.x <= 251)
        {
            if (Position.z >= 437 && Position.z <= 480)
            {
                if (inDoorRange == false)
                {
                    inDoorRange = true;
                    subtitleSize = 0;
                }
                
                std::string text = "Open Door [Press Space]";
                ShowSubtitle(text);
            }
            else
                inDoorRange = false;
        }
        else
            inDoorRange = false;
        
        if (Position.x >= 200 && Position.x <= 253)
        {
            if (Position.z >= -233 && Position.z <= -206)
            {
                if (hasKey == false)
                {
                    if (inKeysRange == false)
                    {
                        inKeysRange = true;
                        subtitleSize = 0;
                    }

                    std::string text = "Pick up the keys. [Press Space]";
                    ShowSubtitle(text);

                    if (InputMgr->KeyDown(VK_SPACE, true))
                        hasKey = true;
                }
            }
            else
                inKeysRange = false;
        }
        else
            inKeysRange = false;
    }
}


void GameManager::ShowSubtitle(std::string str)
{
    static float UpdateSubtitle = 0;

    if (UpdateSubtitle < 0.02f)
        UpdateSubtitle += SceneMgr->GetTimeDelta();
    else
    {
        UpdateSubtitle = 0;

        if (subtitleSize < str.size())
            subtitleSize++;
    }

    RenderMgr->RenderText(str.substr(0, subtitleSize), Vec2(50, RenderMgr->GetHeight() - 100), 50, "Northwood High", 0, FW_BOLD, COLOR::WHITE);
}