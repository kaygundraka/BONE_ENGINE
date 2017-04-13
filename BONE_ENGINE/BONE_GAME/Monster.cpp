#include "Monster.h"
#include <SceneManager.h>
#include <InputManager.h>

void Monster::Init()
{
    status = PATROL;
    speed = 300;
}

void Monster::Reference()
{
    nodes = CUR_SCENE->GetGraphNodes();
 
    auto node = nodes->find("MonsterPathNode1")->second;

    currentNode = node->GetName();
    nextNode = *node->GetConnections().begin();

    moveDir.x = 0;
    moveDir.y = 0;
    moveDir.z = -1;

    auto Pos = ((Transform3D*)node->transform3D)->GetPosition();
    auto Tr = ((Transform3D*)this->gameObject->transform3D);
    Tr->SetPosition(Pos);

    nextNodeTr = (Transform3D*)(*nodes)[*node->GetConnections().begin()]->transform3D;
}

void Monster::Update()
{
    static bool updates = false;

    if (InputMgr->KeyDown('U', true))
        updates = true;

    if (!updates)
        return;

    switch (status) {
    case PATROL: Patrol(); break;
    case SEARCH: Search(); break;
    case COMBAT: Combat(); break;
    }
}

void Monster::Patrol()
{
    if (IsArrived())
    {
        auto node = nodes->find("MonsterPathNode1")->second;

        currentNode = node->GetName();
        nextNode = *node->GetConnections().begin();

        nextNodeTr = (Transform3D*)(*nodes)[*node->GetConnections().begin()]->transform3D;
    }
    else
    {
        GET_RIGIDBODY(this->gameObject)->SetLinearVelocity(
            moveDir.x * speed * SceneMgr->GetTimeDelta(),
            moveDir.y * speed * SceneMgr->GetTimeDelta(),
            moveDir.z * speed * SceneMgr->GetTimeDelta()
        );
    }
}

void Monster::Search()
{

}

void Monster::Combat()
{

}

bool Monster::IsArrived()
{
    auto Tr = ((Transform3D*)this->gameObject->transform3D);

    auto CurPos = Tr->GetPosition();
    auto NextPos = nextNodeTr->GetPosition();

    moveDir = NextPos - CurPos;
    D3DXVec3Normalize(&moveDir, &moveDir);

    CurPos.y = 0;
    NextPos.y = 0;

    if (CurPos.x >= NextPos.x - 10 && CurPos.x <= NextPos.x + 10)
        if (CurPos.z >= NextPos.z - 10 && CurPos.z <= NextPos.z + 10)
            return true;

    return false;
}