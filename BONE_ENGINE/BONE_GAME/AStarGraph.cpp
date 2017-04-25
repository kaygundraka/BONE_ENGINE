#include "AStarGraph.h"

// AStarNode
AStarNode::AStarNode()
{
    graph = CUR_SCENE->GetGraphNodes();
    player = (PlayerCharacter*)(CUR_SCENE->FindObjectByName("Player")->GetComponent("PlayerCharacter"));
}

float AStarNode::BTDistanceFromNode(std::string otherNode)
{
    auto CurNodePos = (*graph)[name]->GetPosition();
    auto OtherNodePos = (*graph)[otherNode]->GetPosition();

    return D3DXVec3Length(&Vec3(CurNodePos - OtherNodePos));
}

float AStarNode::BTDistanceFromPos(Vec3 pos)
{
    auto CurNodePos = (*graph)[name]->GetPosition();
    
    return D3DXVec3Length(&Vec3(CurNodePos - pos));
}

float AStarNode::GetHeuristic()
{
    auto CurNodePos = (*graph)[name]->GetPosition();
    auto PlayerPos = GET_TRANSFORM_3D(player->gameObject)->GetPosition();

    return D3DXVec3Length(&(CurNodePos - PlayerPos));
}

bool AStarNode::IsInRange(Vec3 curPos)
{
    auto NodePos = GetPos();

    if (curPos.x >= NodePos.x - 15 && curPos.x <= NodePos.x + 15)
        if (curPos.z >= NodePos.z - 15 && curPos.z <= NodePos.z + 15)
            return true;

    return false;
}

Vec3 AStarNode::GetPos()
{
    return (*graph)[name]->GetPosition();
}


// AStarGraph
AStarGraph::~AStarGraph()
{
    for (auto iter = nodeList.begin(); iter != nodeList.end();)
    {
        auto pointer = (*iter).second;

        iter = nodeList.erase(iter);

        if (pointer != nullptr)
            delete pointer;
    }
}

void AStarGraph::AddNode(AStarNode* node)
{
    nodeList.insert(std::pair<std::string, AStarNode*>(node->name, node));
}

AStarNode* AStarGraph::GetMinDistNode(Vec3 curPos)
{
    AStarNode* minDistNode;
    float MinDist = 100000;

    for each (auto var in nodeList)
    {
        float Dist = var.second->BTDistanceFromPos(curPos);

        if (Dist < MinDist)
        {
            MinDist = Dist;
            minDistNode = var.second;
        }
    }

    return minDistNode;
}

void AStarGraph::PathFinding(Vec3 curPos)
{
    auto Player = (PlayerCharacter*)(CUR_SCENE->FindObjectByName("Player")->GetComponent("PlayerCharacter"));
    auto PlayerPos = GET_TRANSFORM_3D(Player->gameObject)->GetPosition();

    auto DestNode= GetMinDistNode(PlayerPos);
    AStarNode* CurNode = GetMinDistNode(curPos);
    path.push(CurNode);

    while (true)
    {
        if (CurNode == DestNode)
            break;

        AStarNode* NextNode = nullptr;

        for each (auto var in CurNode->neighborList)
        {
            if (NextNode == nullptr)
                NextNode = var;
            else
            {
                float result1 = NextNode->GetHeuristic() + CurNode->BTDistanceFromNode(NextNode->name);
                float result2 = var->GetHeuristic() + CurNode->BTDistanceFromNode(var->name);

                if (result1 > result2)
                    NextNode = var;
            }
        }

        if (NextNode != nullptr)
        {
            CurNode = NextNode;
            path.push(NextNode);
        }
    }
}

std::queue<AStarNode*> AStarGraph::GetPath()
{
    return path;
}

std::map<std::string, AStarNode*>* AStarGraph::GetNodeList()
{
    return &nodeList;
}