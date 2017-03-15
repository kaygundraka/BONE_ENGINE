#include "Common.h"
#include "Script.h"

namespace BONE_GRAPHICS {

    Script::Script(GameObject* gameObject, std::string name)
    {
        this->gameObject = gameObject;
        this->SetTypeName(name);

        isScript = true;
    }

    Script::~Script()
    {

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

    void Script::OnCollisionEnter(GameObject* otherObject)
    {

    }

    void Script::OnCollisionStay(GameObject* otherObject)
    {

    }

    void Script::OnCollisionLeave(GameObject* otherObject)
    {

    }
}