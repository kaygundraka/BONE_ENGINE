#pragma once
#include <Script.h>
#include <Transform3D.h>
#include <GraphNode.h>
#include <Rp3dRigidBody.h>
#include <SkinnedMesh.h>
#include <StaticMesh.h>
#include <SoundClip.h>
#include "PlayerCharacter.h"
#include "AStarGraph.h"
using namespace BONE_GRAPHICS;

class Monster : public Script {
public:
    enum MONSTER_STATUS {
        PATROL, SEARCH, COMBAT, FOLLOW
    };

private:
    AStarGraph pathGraph;

    std::map<std::string, GraphNode*>* nodes;

    Transform3D* transform;
    SoundClip* soundClips;

    float hp;

    bool damaged;
    float attackTime;

    double sleepTimer;
    Vec3 moveDir;
    Vec3 preDir;
    Vec3 curPos;
    Vec3 playerPos;
    Vec3 nextPos;

    double searchTimer;

    bool failAttack;
    bool isAttack;
    float attackTiming;
    float rotateYAngle;

    PlayerCharacter* player;

    float speed;

    MONSTER_STATUS status;
    AStarNode* patrollNode;

    BONE_GRAPHICS::RigidBody* rigidBody;
    SkinnedMesh* skinnedMesh;

    void Patrol();
    void Search();
    void Combat();
    void Follow();

    bool SearchAlgorithm(float range, float angle);
    float CalcDirDist(Vec3 to);

public:
    void Damaged(int damage);
    float GetAttackTiming();

    virtual void Init();
    virtual void Reference();
    virtual void Update();
};
