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
	class ScreenButton : public Component
	{
	public:
		enum BUTTON_STATUS { NORMAL, PRESSED, HIGHLIGHT };

	protected:
		LPD3DXSPRITE sprite;
		Rect originRect;
		std::string address;
		float alpha;

		BUTTON_STATUS status;

	public:
		ScreenButton();
		virtual ~ScreenButton();

		void LoadContent();

		BUTTON_STATUS Update(GameObject* owner);

		void SetOriginRect(Rect rect);
		void SetOriginRect(Vector2 leftTop, Vector2 rightBottom);
		Rect GetOriginRect();

		void SetImageFile(std::string address);
		
		void SetAlpha(float alpha);
		virtual void Render(GameObject* owner);
		float GetAlpha();
	};
}