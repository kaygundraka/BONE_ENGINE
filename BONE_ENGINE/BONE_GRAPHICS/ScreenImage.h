#pragma once
#include "Common.h"
#include "Component.h"
#include "GameObject.h"
#include <MultiThreadSync.h>
using namespace BONE_SYSTEM;

namespace BONE_GRAPHICS
{
	class ScreenImage : public Component, public CMultiThreadSync<ScreenImage> 
	{
	protected:
		LPD3DXSPRITE sprite;
		Rect originRect;
		string address;
		float alpha;

	public:
		ScreenImage();
		~ScreenImage();

		void LoadContent();

		void SetOriginRect(Rect _rect);
		void SetOriginRect(Vector2 _leftTop, Vector2 _rightBottom);
		Rect GetOriginRect();

		void SetImageFile(string _address);
		
		void SetAlpha(float _alpha);
		virtual void Render(GameObject* _owner);
		float GetAlpha();
	};
}