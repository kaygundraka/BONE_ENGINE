#pragma once
#include "Common.h"
#include "Component.h"
#include "GameObject.h"
using namespace BONE_SYSTEM;

namespace BONE_GRAPHICS
{
	class ScreenSprite : public Component
	{
	protected:
		int width;
		int height;
		int animeCut;
		int animeScene;
		float alpha;
		bool IsShow;
		int curAnimeCut;
		int curAnimeScene;
		bool rightPlay;

		LPD3DXSPRITE sprite;
		string address;
		RECT curRect;

	public:
		ScreenSprite();
		~ScreenSprite();

		void LoadContent();

		void SetInformation(string address, int width, int height, int animationCut, int animationScene, float alpha);

		void PlayAnimation(float cutTimer);
		void SelectAnimation(int sceneIndex);
		void SetPlayDirection(bool isRight);

		void SetAlpha(float alpha);

		virtual void Render(GameObject* owner);
		
		float GetAlpha();
	};
}