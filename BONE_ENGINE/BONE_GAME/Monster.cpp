#include "Monster.h"
#include <SceneManager.h>
#include <InputManager.h>
#include <RandomGenerator.h>

void Monster::Init()
{
    status = PATROL;
    speed = 1500;

    soundClips = new SoundClip();
    soundClips->AttachObject(this->gameObject);
    soundClips->AddClip("footstep.mp3", 0.6f, true, false, 50.0f, 0.0f);
    this->gameObject->AddComponent(soundClips);
}

void Monster::Reference()
{
    nodes = CUR_SCENE->GetGraphNodes();
 
    auto node = nodes->find("MapPath0")->second;

    currentNode = node->GetName();
    nextNode = *node->GetConnections().begin();

    moveDir.x = 0;
    moveDir.y = 0;
    moveDir.z = -1;

    auto Pos = ((Transform3D*)node->transform3D)->GetPosition();
    auto Tr = ((Transform3D*)this->gameObject->transform3D);
    Tr->SetPosition(Pos);

    nextNodeTr = (Transform3D*)(*nodes)[*node->GetConnections().begin()]->transform3D;

    skinnedMesh = GET_SKINNED_MESH(this->gameObject);

    GameObject* Sword = new GameObject();
    Sword->SetPriority(1);

    Transform3D* SwordTransform = new Transform3D();
    auto BoneMatrix = skinnedMesh->GetBoneMatrix("hand_r");
    SwordTransform->CombineMatrix(BoneMatrix);
    SwordTransform->SetRotate(-0.45f, 0.96f, 1.85f);
    SwordTransform->SetScale(2.0f, 2.0f, 2.0f);
    Sword->AddComponent(SwordTransform);
    
    StaticMesh* SwordMesh = new StaticMesh();
    SwordMesh->SetFile("Sword.X");
    Sword->AddComponent(SwordMesh);

    Sword->AttachParent(this->gameObject);

    CUR_SCENE->AddObject(Sword, "MonsterSword");

    GameObject* shield = new GameObject();
    shield->SetPriority(1);

    Transform3D* ShieldTransform = new Transform3D();
    auto ShieldMatrix = skinnedMesh->GetBoneMatrix("hand_l");
    ShieldTransform->CombineMatrix(ShieldMatrix);
    ShieldTransform->SetPosition(0.0f, 0.0f, 0.0f);
    ShieldTransform->SetRotate(-0.45f, 0.96f, 1.85f);
    ShieldTransform->SetScale(2.0f, 2.0f, 2.0f);
    shield->AddComponent(ShieldTransform);
    
    StaticMesh* ShieldMesh = new StaticMesh();
    ShieldMesh->SetFile("Shield.X");
    shield->AddComponent(ShieldMesh);

    shield->AttachParent(this->gameObject);

    CUR_SCENE->AddObject(shield, "PlayerShield");

    transform = (Transform3D*)this->gameObject->transform3D;
    rigidBody = GET_RIGIDBODY(this->gameObject);
 
    rotateYAngle = 0;
    sleepTimer = 0;

    player = (PlayerCharacter*)(CUR_SCENE->FindObjectByName("Player")->GetComponent("PlayerCharacter"));
}

void Monster::Update()
{
    if (CUR_SCENE->IsEnablePhysics() == false)
        return;

    if (sleepTimer < 5.0f)
    {
        sleepTimer += SceneMgr->GetTimeDelta();
        return;
    }

   /* switch (status) {
    case PATROL: Patrol(); break;
    case SEARCH: Search(); break;
    case COMBAT: Combat(); break;
    }*/
}

float GetAngle(const Vec3& a, const Vec3& b)
{
    float cosAngle = D3DXVec3Dot(&a, &b);

    return (a.z*b.x + a.x*b.z > 0.0f) ? (float)(acos(cosAngle)) : -(float)(acos(cosAngle));
}

void Monster::Patrol()
{
    if (!soundClips->IsPlaying("footstep.mp3"))
        soundClips->Play("footstep.mp3");

    skinnedMesh->SetAnimationLoop(true);
    skinnedMesh->SetAnimation("Skeleton_1H_walk");

    CurPos = transform->GetPosition();
    NextPos = nextNodeTr->GetPosition();

    CurPos.y = 0;
    NextPos.y = 0;

    moveDir = NextPos - CurPos;
    D3DXVec3Normalize(&moveDir, &moveDir);
    preDir = moveDir;

    if (CurPos.x >= NextPos.x - 10 && CurPos.x <= NextPos.x + 10 
        && CurPos.z >= NextPos.z - 10 && CurPos.z <= NextPos.z + 10)
    {
        auto node = nodes->find(nextNode)->second;

        currentNode = node->GetName();
        nextNode = *node->GetConnections().begin();

        nextNodeTr = (Transform3D*)(*nodes)[*node->GetConnections().begin()]->transform3D;

        NextPos = nextNodeTr->GetPosition();
        NextPos.y = 0;

        moveDir = NextPos - CurPos;
        D3DXVec3Normalize(&moveDir, &moveDir);

        rotateYAngle = GetAngle(Vec3(0, 0, -1), moveDir);
        rigidBody->SetTransform(transform->GetPosition(), Vec3(0, rotateYAngle, 0));
    }
    else
    {
        rigidBody->SetLinearVelocity(
            moveDir.x * speed * SceneMgr->GetTimeDelta(),
            moveDir.y * speed * SceneMgr->GetTimeDelta(),
            -moveDir.z * speed * SceneMgr->GetTimeDelta()
        );

        auto PlayerTr = GET_TRANSFORM_3D(player->gameObject);
        auto PlayerPos = PlayerTr->GetWorldPositon();
        PlayerPos.y = 0;

        float Distance = D3DXVec3Length(&(PlayerPos - CurPos));

        if (player->IsSneakingMode() && Distance < 60)
        {
            status = SEARCH;
            rigidBody->SetLinearVelocity(0, 0, 0);
        }
        else if (Distance < 100)
        {
            status = SEARCH;
            rigidBody->SetLinearVelocity(0, 0, 0);
        }
    }
}

void Monster::Search()
{
    skinnedMesh->SetAnimation("Skeleton_Looking_around");

    auto PlayerTr = GET_TRANSFORM_3D(player->gameObject);
    auto PlayerPos = PlayerTr->GetWorldPositon();

    CurPos = transform->GetPosition();

    CurPos.y = 0;
    PlayerPos.y = 0;

    float Distance = D3DXVec3Length(&(PlayerPos - CurPos));

    if (player->IsSneakingMode() && Distance >= 60)
        status = PATROL;
    else if (Distance >= 100)
        status = PATROL;

    int FindHeuristics = 0;

    if (player->IsSneakingMode())
        FindHeuristics = RandomGenerator::GetRandInt(0, 100);
    else
        FindHeuristics = RandomGenerator::GetRandInt(0, 80);

    if (FindHeuristics < 5 || Distance <= 30)
    {
        status = COMBAT;
        skinnedMesh->SetAnimationLoop(false);
        skinnedMesh->SetAnimation("Skeleton_anger");
    }
}

void Monster::Combat()
{
    if (skinnedMesh->GetAnimationRate() >= 0.99f && skinnedMesh->GetCurrentAnimation() == "Skeleton_anger")
    {
        skinnedMesh->SetAnimationLoop(true);
        skinnedMesh->SetAnimation("Skeleton_1H_combat_mode");
    }

    auto PlayerTr = GET_TRANSFORM_3D(player->gameObject);
    auto PlayerPos = PlayerTr->GetWorldPositon();
    
    CurPos = transform->GetPosition();
    
    CurPos.y = 0;
    PlayerPos.y = 0;
    
    auto Dir = PlayerPos - CurPos;
    
    float Distance = D3DXVec3Length(&Dir);
    D3DXVec3Normalize(&Dir, &Dir);

    // 몬스터 회피기 Skeleton_1H_dodge_backwards
    // Skeleton_Hit_from_back
    // Skeleton_Hit_from_front
    // Skeleton_Dying_B

    if (Distance >= 200)
        status = PATROL;
    else if (Distance <= 30)
    {
        rigidBody->SetLinearVelocity(0, 0, 0);
        skinnedMesh->SetAnimationLoop(true);
        skinnedMesh->SetAnimation("Skeleton_1H_swing_left");
    }
    else
    {
        skinnedMesh->SetAnimationLoop(true);
        skinnedMesh->SetAnimation("Skeleton_1H_charging");

        rigidBody->SetLinearVelocity(
            Dir.x * speed * 2 * SceneMgr->GetTimeDelta(),
            Dir.y * speed * 2 * SceneMgr->GetTimeDelta(),
            -Dir.z * speed * 2 * SceneMgr->GetTimeDelta()
        );
    }

}