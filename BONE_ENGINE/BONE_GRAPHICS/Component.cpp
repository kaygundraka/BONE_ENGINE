#include "Common.h"
#include "Component.h"

namespace BONE_GRAPHICS
{
	void Component::SetTypeName(string _typeName)
	{
		typeName = _typeName;
	}

	string Component::GetTypeName() 
	{
		return typeName;
	}

	void Component::LoadContent()
	{

	}
}