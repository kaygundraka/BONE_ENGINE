#pragma once
#include "Common.h"
#include "GameObject.h"
#include "Transform3D.h"

namespace BONE_GRAPHICS
{
    class GraphNode : public GameObject
    {
    private:
        Transform3D* tr;
        std::list<std::string> nodes;

        bool showIcon;

    public:
        GraphNode();
        virtual ~GraphNode();

        void SetPosition(Vec3 pos);
        Vec3 GetPosition();

        void ConnectNode(std::string node);
        void DisconnectNode(std::string node);

        void SetIcon(bool show);
        bool ShowIcon();

        virtual void SaveInMaps();

        virtual void Init();
        virtual void Reference();
    };
}