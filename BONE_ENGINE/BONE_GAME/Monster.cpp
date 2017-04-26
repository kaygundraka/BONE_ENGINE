#include "Monster.h"
#include "Function.h"
#include <SceneManager.h>
#include <InputManager.h>
#include <RandomGenerator.h>

void Monster::Init()
{
    status = PATROL;
    speed = 1000;
    hp = 100;

    soundClips = new SoundClip();
    soundClips->AttachObject(this->gameObject);
    soundClips->AddClip("footstep.mp3", 1.0f, true, false, 50.0f, 0.0f);
    this->gameObject->AddComponent(soundClips);
}

void Monster::Reference()
{
    nodes = CUR_SCENE->GetGraphNodes();

    for each(auto var in *nodes)
    {
        AStarNode* astarNode = new AStarNode();
        astarNode->name = var.second->GetName();
        pathGraph.AddNode(astarNode);
    }

    auto NodeList = pathGraph.GetNodeList();

    for each(auto var in *nodes)
        for each (auto var2 in var.second->GetConnections())
            (*NodeList)[var.first]->neighborList.push_back((*NodeList).find(var2)->second);

    moveDir.x = 0;
    moveDir.y = 0;
    moveDir.z = -1;
        
    auto Tr = ((Transform3D*)this->gameObject->transform3D);
    auto StartNode = pathGraph.GetMinDistNode(Tr->GetPosition());

    Tr->SetPosition(StartNode->GetPos());
    patrollNode = StartNode;


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

    if (hp <= 0)
    {
        skinnedMesh->SetAnimationLoop(false);
        skinnedMesh->SetAnimation("Skeleton_Dying_B");

        return;
    }

    if (sleepTimer < 5.0f)
    {
        sleepTimer += SceneMgr->GetTimeDelta();
        return;
    }

    auto PlayerTr = GET_TRANSFORM_3D(player->gameObject);
    playerPos = PlayerTr->GetWorldPositon();
    playerPos.y = 0;

    curPos = transform->GetPosition();
    curPos.y = 0;

    if (damaged)
    {
        Vec3 BetweenDir = playerPos - curPos;
        D3DXVec3Normalize(&BetweenDir, &BetweenDir);

        float Angle = GetAngle(moveDir, BetweenDir);

        if (Angle < 0)
            Angle = -Angle;

        rigidBody->SetLinearVelocity(0, 0, 0);

        if (Angle >= 1.57f)
        {
            skinnedMesh->SetAnimationLoop(false);
            skinnedMesh->SetAnimation("Skeleton_Hit_from_back");
        }
        else
        {
            skinnedMesh->SetAnimationLoop(false);
            skinnedMesh->SetAnimation("Skeleton_Hit_from_front");
        }

        if (skinnedMesh->GetCurrentAnimation() == "Skeleton_Hit_from_back" || skinnedMesh->GetCurrentAnimation() == "Skeleton_Hit_from_front")
            if (skinnedMesh->GetAnimationRate() >= 0.99f)
                damaged = false;
    }
    else
    {
        switch (status) {
        case PATROL: Patrol(); break;
        case SEARCH: Search(); break;
        case COMBAT: Combat(); break;
        case FOLLOW: Follow(); break;
        }
    }
}

void Monster::Patrol()
{
    if (!soundClips->IsPlaying("footstep.mp3"))
        soundClips->Play("footstep.mp3");

    skinnedMesh->SetAnimationLoop(true);
    skinnedMesh->SetAnimation("Skeleton_1H_walk");
    
    if (patrollNode->IsInRange(curPos))
        patrollNode = patrollNode->neighborList[RandomGenerator::GetRandInt(0, patrollNode->neighborList.size() - 1)];
    
    Vec3 NextPos = patrollNode->GetPos();
    NextPos.y = 0;
    
    CalcDirDist(NextPos);

    rotateYAngle = GetAngle(Vec3(0, 0, -1), moveDir);
    rigidBody->SetTransform(transform->GetPosition(), Vec3(0, rotateYAngle, 0));

    rigidBody->SetLinearVelocity(
        moveDir.x * speed * SceneMgr->GetTimeDelta(),
        moveDir.y * speed * SceneMgr->GetTimeDelta(),
        -moveDir.z * speed * SceneMgr->GetTimeDelta()
    );

    /// 거리에 따른 탐색 변화
    float Distance = CalculateDistance(curPos, playerPos);

    if (player->IsSneakingMode() && SearchAlgorithm(60, PI / 2.5f))
    {
        status = FOLLOW;
        pathGraph.PathFinding(curPos);
        rigidBody->SetLinearVelocity(0, 0, 0);
    }
    else if (SearchAlgorithm(90, PI / 2))
    {
        status = FOLLOW;
        pathGraph.PathFinding(curPos);
        rigidBody->SetLinearVelocity(0, 0, 0);
    }
    else if (Distance < 40)
    {
        status = SEARCH;
        searchTimer = RandomGenerator::GetRandInt(3, 5);
        rigidBody->SetLinearVelocity(0, 0, 0);
    }
}

bool Monster::SearchAlgorithm(float range, float angle)
{
    bool result = true;

    float Distance = CalculateDistance(curPos, playerPos);

    if (Distance > range)
        result = false;

    Vec3 BetweenDir = playerPos - curPos;
    D3DXVec3Normalize(&BetweenDir, &BetweenDir);

    float Angle = GetAngle(moveDir, BetweenDir);

    if (Angle < 0)
        Angle = -Angle;

    if (Angle > angle)
        result = false;

    return result;
}

void Monster::Search()
{
    skinnedMesh->SetAnimation("Skeleton_Looking_around");

    float Distance = CalculateDistance(curPos, playerPos);
    
    if (searchTimer > 0)
    {
        searchTimer -= SceneMgr->GetTimeDelta();
        return;
    }
    else
        searchTimer = 0;

    int FindHeuristics = 0;

    if (player->IsSneakingMode())
        FindHeuristics = RandomGenerator::GetRandInt(0, 100);
    else
        FindHeuristics = RandomGenerator::GetRandInt(0, 80);

    if (FindHeuristics < 30 || Distance <= 30)
    {
        status = FOLLOW;
        pathGraph.PathFinding(curPos);
    }
}

void Monster::Follow()
{
    skinnedMesh->SetAnimationLoop(true);
    skinnedMesh->SetAnimation("Skeleton_1H_charging");
    
    auto Path = pathGraph.GetPath();

    if (Path->size() != 0 && Path->front()->IsInRange(curPos))
        pathGraph.PathFinding(curPos);
    
    Vec3 NextPos;

    if (Path->size() == 0)
    {
        pathGraph.PathFinding(curPos);

        float Distance = CalculateDistance(playerPos, curPos);

        if (Path->size() != 0 && Distance >= 100)
            return;
                
        NextPos = GET_TRANSFORM_3D(player->gameObject)->GetPosition();
            
        if (Distance < 40)
        {
            rigidBody->SetLinearVelocity(0, 0, 0);
            status = COMBAT;
        }
    }
    else
        NextPos = Path->front()->GetPos();
    
    NextPos.y = 0;
    
    CalcDirDist(NextPos);

    rotateYAngle = GetAngle(Vec3(0, 0, -1), moveDir);
    rigidBody->SetTransform(transform->GetPosition(), Vec3(0, rotateYAngle, 0));

    rigidBody->SetLinearVelocity(
        moveDir.x * speed * 1.5f * SceneMgr->GetTimeDelta(),
        moveDir.y * speed * 1.5f * SceneMgr->GetTimeDelta(),
        -moveDir.z * speed * 1.5f * SceneMgr->GetTimeDelta()
    );
}

// return distance
float Monster::CalcDirDist(Vec3 to)
{
    moveDir = to - curPos;

    float Distance = D3DXVec3Length(&moveDir);
    D3DXVec3Normalize(&moveDir, &moveDir);

    return Distance;
}

void Monster::Combat()
{
    auto Distance = CalcDirDist(playerPos);

    if (Distance <= 40)
    {
        rotateYAngle = GetAngle(Vec3(0, 0, -1), moveDir);
        rigidBody->SetTransform(transform->GetPosition(), Vec3(0, rotateYAngle, 0));
        rigidBody->SetLinearVelocity(0, 0, 0);
        
        skinnedMesh->SetAnimationLoop(true);
        skinnedMesh->SetAnimation("Skeleton_1H_swing_left");
    }
    else
    {
        status = FOLLOW;
        pathGraph.PathFinding(curPos);
    }
}

void Monster::Damaged(int damage)
{
    if (CalculateDistance(playerPos, curPos) <= 40)
    {
        damaged = true;
        hp -= damage;
    }
}