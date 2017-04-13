#pragma once
#include <Script.h>
#include <Transform3D.h>
#include <GraphNode.h>
#include <Rp3dRigidBody.h>
using namespace BONE_GRAPHICS;

class Monster : public Script {
public:
    enum MONSTER_STATUS {
        PATROL, SEARCH, COMBAT
    };

private:
    std::map<std::string, GraphNode*>* nodes;

    Transform3D* nextNodeTr;
    Vec3 moveDir;

    std::string currentNode;
    std::string nextNode;

    float speed;

    MONSTER_STATUS status;

    bool IsArrived();

    void Patrol();
    void Search();
    void Combat();

public:
    virtual void Init();
    virtual void Reference();
    virtual void Update();
};
