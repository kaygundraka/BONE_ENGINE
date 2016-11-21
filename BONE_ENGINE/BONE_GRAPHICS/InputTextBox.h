#pragma once
#include "Common.h"
#include "ResourceManager.h"
#include "IShader.h"
#include "Component.h"
#include "GameObject.h"
#include "Transform2D.h"
#include <MultiThreadSync.h>
using namespace BONE_SYSTEM;

namespace BONE_GRAPHICS
{
	class InputTextBox : public Component, public CMultiThreadSync<InputTextBox>
	{
	private:
		bool active;
		bool isFocused;
		bool isClicked;
		string str;
		int maxStrLength;

	public:
		InputTextBox();
		virtual ~InputTextBox();

		void LoadContent();
	
		bool IsClicked(GameObject* _owner);
		void SetActive(bool _active);
		string GetString();
		void ClearString();

		void SetMaxLength(int _length);
		int GetMaxLength();
		
		void Update(GameObject* _owner, float _timeDelta);
	};
}