#include "Common.h"
#include "Component.h"

namespace BONE_GRAPHICS
{
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
}