#include "Common.h"
#include "Component.h"

namespace BONE_GRAPHICS
{
	void Component::SetTypeName(string typeName)
	{
		this->typeName = typeName;
	}

	string Component::GetTypeName() 
	{
		return typeName;
	}

	void Component::LoadContent()
	{

	}
}