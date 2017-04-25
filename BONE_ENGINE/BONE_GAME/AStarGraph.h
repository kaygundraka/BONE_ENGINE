#pragma once
#include <GraphNode.h>
#include <SceneManager.h>
#include <queue>
#include "PlayerCharacter.h"
using namespace BONE_GRAPHICS;

class AStarNode {
private:
    PlayerCharacter* player;
    std::map<std::string, BONE_GRAPHICS::GraphNode*>* graph;

public:
    std::string name;
    std::vector<AStarNode*> neighborList;

    AStarNode();

    float BTDistanceFromNode(std::string otherNode);
    float BTDistanceFromPos(Vec3 pos);
    float GetHeuristic();
    bool IsInRange(Vec3 curPos);

    Vec3 GetPos();
};

class AStarGraph {
private:
    std::map<std::string, AStarNode*> nodeList;
    std::queue<AStarNode*> path;

    Vec3 direction;

public:
    ~AStarGraph();

    std::map<std::string, AStarNode*>* GetNodeList();

    void AddNode(AStarNode* node);
    void PathFinding(Vec3 curPos);

    AStarNode* GetMinDistNode(Vec3 curPos);
    std::queue<AStarNode*>* GetPath();
};


