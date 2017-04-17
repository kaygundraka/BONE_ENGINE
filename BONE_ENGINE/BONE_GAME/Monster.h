#pragma once
#include <Script.h>
#include <Transform3D.h>
#include <GraphNode.h>
#include <Rp3dRigidBody.h>
#include <SkinnedMesh.h>
#include <StaticMesh.h>
#include <SoundClip.h>
using namespace BONE_GRAPHICS;

class Monster : public Script {
public:
    enum MONSTER_STATUS {
        PATROL, SEARCH, COMBAT
    };

private:
    std::map<std::string, GraphNode*>* nodes;

    Transform3D* nextNodeTr;
    Transform3D* transform;
    SoundClip* soundClips;

    float sleepTimer;
    Vec3 moveDir;
    Vec3 preDir;
    Vec3 CurPos;
    Vec3 NextPos;

    float rotateYAngle;

    std::string currentNode;
    std::string nextNode;

    float speed;

    MONSTER_STATUS status;

    BONE_GRAPHICS::RigidBody* rigidBody;
    SkinnedMesh* skinnedMesh;

    void Patrol();
    void Search();
    void Combat();

public:
    virtual void Init();
    virtual void Reference();
    virtual void Update();
};
