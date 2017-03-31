#pragma once
#include "Common.h"
#include "Component.h"
#include "GameObject.h"
using namespace BONE_SYSTEM;

namespace BONE_GRAPHICS
{
	class ScreenImage : public Component
	{
	protected:
		LPD3DXSPRITE sprite;
		Rect originRect;
		std::string fileName;
		float myalpha;

	public:
		ScreenImage();
		~ScreenImage();

		void LoadContent();

		void SetOriginRect(Rect rect);
		void SetOriginRect(Vec2 leftTop, Vec2 rightBottom);
		Rect GetOriginRect();

		void SetImageFile(std::string fileName);
		
		void SetAlpha(float alpha);
		virtual void Render(GameObject* owner);
		float GetAlpha();
	};
}