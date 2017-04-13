#include "Common.h"
#include "GraphNode.h"
#include "BillBoard.h"
#include "SceneManager.h"

namespace BONE_GRAPHICS
{
    GraphNode::GraphNode()
    {
        tr = nullptr;
        showIcon = false;
    }

    GraphNode::~GraphNode() {}

    void GraphNode::Init()
    {
        SetTag("GraphNode");
        SetPriority(100);

        BillBoard* billBoard = new BillBoard();
        Rect rect;
        rect.LeftTop = Vec2(0, 0);
        rect.RightBottom = Vec2(512, 512);
        billBoard->SetOriginRect(rect);
        billBoard->SetRectSize(10, 10);
        billBoard->SetTexture("GraphNode.png", 512, 512);
        billBoard->SetRenderMode(BillBoard::RENDER_ALPHA);
        this->AddComponent(billBoard);

        Transform3D* tr = new Transform3D();
        this->AddComponent(tr);
        this->tr = (Transform3D*)transform3D;

        billBoard->SetShow(false);
        this->SetPipeLine(GameObject::DIRECT_DEFAULT);
    }

    void GraphNode::Reference()
    {
        GET_BILLBOARD(this)->SetTarget(SceneMgr->CurrentScene()->GetCurrentCamera());
    }

    void GraphNode::SetPosition(Vec3 pos)
    {
        tr->SetPosition(pos);
    }

    Vec3 GraphNode::GetPosition()
    {
        return tr->GetPosition();
    }

    void GraphNode::SetIcon(bool show)
    {
        ((BillBoard*)GetComponent("BillBoard"))->SetShow(show);

        showIcon = show;
    }
    
    bool GraphNode::ShowIcon()
    {
        return showIcon;
    }

    void GraphNode::SaveInMaps()
    {
        bool isNewMap = false;
        std::string FullPath;

        if (!ResourceMgr->ExistFile(SceneMgr->CurrentScene()->GetName() + ".json", &FullPath))
            isNewMap = true;

        json j;

        std::ifstream file(FullPath);

        if (!isNewMap)
            file >> j;

        j["GraphNode"][name]["Position"] = {
            ((Transform3D*)transform3D)->GetPosition().x,
            ((Transform3D*)transform3D)->GetPosition().y,
            ((Transform3D*)transform3D)->GetPosition().z
        };

        for each (auto var in nodes)
            j["GraphNode"][name]["Connections"].push_back(var);

        if (!isNewMap)
            file.close();

        std::string fullPath = ".\\Engine\\Maps\\" + SceneMgr->CurrentScene()->GetName() + ".json";
        std::ofstream o(fullPath);
        o << std::setw(4) << j << std::endl;
        o.close();
    }

    void GraphNode::ConnectNode(std::string node)
    {
        nodes.push_back(node);
    }
    
    void GraphNode::DisconnectNode(std::string node)
    {
        for (auto iter = nodes.begin(); iter != nodes.end(); iter++)
        {
            if (*iter == node)
            {
                nodes.erase(iter);
                break;
            }
        }
    }

    std::list<std::string> GraphNode::GetConnections()
    {
        return nodes;
    }
}