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

    if (curPos.x >= NodePos.x - 10 && curPos.x <= NodePos.x + 10)
        if (curPos.z >= NodePos.z - 10 && curPos.z <= NodePos.z + 10)
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
    int Size = path.size();
    for (int i = 0; i < Size; i++)
        path.pop();

    auto Player = (PlayerCharacter*)(CUR_SCENE->FindObjectByName("Player")->GetComponent("PlayerCharacter"));
    auto PlayerPos = GET_TRANSFORM_3D(Player->gameObject)->GetPosition();

    auto DestNode= GetMinDistNode(PlayerPos);
    auto CurNode = GetMinDistNode(curPos);

    std::set<std::string> VisitedList;
    VisitedList.insert(CurNode->name);
    
    while (true)
    {
        if (CurNode == DestNode)
            break;

        if (CurNode->neighborList.size() == 0)
            break;

        AStarNode* NextNode = nullptr;
        
        for each (auto var in CurNode->neighborList)
        {
            if (VisitedList.find(var->name) != VisitedList.end())
                continue;

            if (NextNode == nullptr)
                NextNode = var;
            else
            {
                float f1 = NextNode->GetHeuristic() + CurNode->BTDistanceFromNode(NextNode->name);
                float f2 = var->GetHeuristic() + CurNode->BTDistanceFromNode(var->name);

                if (f1 > f2)
                    NextNode = var;
            }
        }

        if (NextNode != nullptr)
        {
            CurNode = NextNode;
            path.push(NextNode);
            VisitedList.insert(NextNode->name);
        }
        else
            break;
    }
}

std::queue<AStarNode*>* AStarGraph::GetPath()
{
    return &path;
}

std::map<std::string, AStarNode*>* AStarGraph::GetNodeList()
{
    return &nodeList;
}