#pragma once
#include "Common.h"
#include "ResourceManager.h"
#include "IShader.h"
#include "Component.h"
#include "GameObject.h"
#include "Transform2D.h"
using namespace BONE_SYSTEM;

namespace BONE_GRAPHICS
{
	class InputTextBox : public Component
    {
	private:
		bool active;
		bool isFocused;
		bool isClicked;
		std::string str;
		int maxStrLength;

	public:
		InputTextBox();
		virtual ~InputTextBox();

		void LoadContent();
	
		bool IsClicked(GameObject* owner);
		void SetActive(bool active);
		std::string GetString();
		void ClearString();

		void SetMaxLength(int length);
		int GetMaxLength();
		
		void Update(GameObject* owner);
	};
}