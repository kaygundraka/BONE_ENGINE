#pragma once

namespace BONE_GRAPHICS
{
	class Component{
	private:
		string typeName;
		
	protected:
		void SetTypeName(string _typeName);

	public:
		virtual void LoadContent();
		string GetTypeName();

		virtual ~Component() {}
	};
}