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
	class ScreenButton : public Component, public MultiThreadSync<ScreenButton>
	{
	public:
		enum BUTTON_STATUS { NORMAL, PRESSED, HIGHLIGHT };

	protected:
		LPD3DXSPRITE sprite;
		Rect originRect;
		string address;
		float alpha;

		BUTTON_STATUS status;

	public:
		ScreenButton();
		virtual ~ScreenButton();

		void LoadContent();

		BUTTON_STATUS Update(GameObject* _owner, float _timeDelta);

		void SetOriginRect(Rect _rect);
		void SetOriginRect(Vector2 _leftTop, Vector2 _rightBottom);
		Rect GetOriginRect();

		void SetImageFile(string _address);
		
		void SetAlpha(float _alpha);
		virtual void Render(GameObject* _owner);
		float GetAlpha();
	};
}