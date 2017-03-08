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
		std::string address;
		float alpha;

	public:
		ScreenImage();
		~ScreenImage();

		void LoadContent();

		void SetOriginRect(Rect rect);
		void SetOriginRect(Vector2 leftTop, Vector2 rightBottom);
		Rect GetOriginRect();

		void SetImageFile(std::string address);
		
		void SetAlpha(float alpha);
		virtual void Render(GameObject* owner);
		float GetAlpha();
	};
}