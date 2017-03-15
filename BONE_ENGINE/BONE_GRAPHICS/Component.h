#pragma once
#include "Common.h"

namespace BONE_GRAPHICS
{
	class Component{
	private:
		std::string typeName;
        
	protected:
		void SetTypeName(std::string typeName);
        bool isScript;

	public:
        Component();
        
		virtual void LoadContent();
        std::string GetTypeName();

        bool IsScript();

		virtual ~Component() {}
	};
}