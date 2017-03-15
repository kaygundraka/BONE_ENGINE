#pragma once
#include "Common.h"

namespace BONE_GRAPHICS
{
	class Component{
	private:
		std::string typeName;
		
	protected:
		void SetTypeName(std::string typeName);

	public:
		virtual void LoadContent();
        std::string GetTypeName();

		virtual ~Component() {}
	};
}