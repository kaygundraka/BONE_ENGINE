#include "Common.h"
#include "Component.h"

namespace BONE_GRAPHICS
{
    Component::Component()
    {
        isScript = false;
    }

	void Component::SetTypeName(std::string typeName)
	{
		this->typeName = typeName;
	}

    std::string Component::GetTypeName()
	{
		return typeName;
	}

	void Component::LoadContent()
	{

	}

    bool Component::IsScript()
    {
        return isScript;
    }
}