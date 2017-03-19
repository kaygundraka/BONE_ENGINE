#include "Common.h"
#include "Script.h"

namespace BONE_GRAPHICS {

    Script::Script()
    {
    }

    Script::~Script()
    {

    }

    void Script::SetInfo(GameObject* gameObject, std::string name)
    {
        this->gameObject = gameObject;
        this->SetTypeName(name);

        isScript = true;
    }

    void Script::Awake()
    {

    }

    void Script::Init()
    {

    }

    void Script::Reference()
    {

    }

    void Script::Update()
    {

    }

    void Script::LateUpdate()
    {

    }

    void Script::LateRender()
    {

    }

    void Script::CollisionEvent(GameObject* otherObject)
    {

    }
}