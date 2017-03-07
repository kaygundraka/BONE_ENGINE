#pragma once
#include "Common.h"
#include "Component.h"
#include "GameObject.h"
#include <MultiThreadSync.h>
using namespace BONE_SYSTEM;

namespace BONE_GRAPHICS
{
	class ScreenSprite : public Component, public MultiThreadSync<ScreenSprite>
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

		void SetInformation(string _address, int _width, int _height, int _animationCut, int _animationScene, float _alpha);

		void PlayAnimation(float _timeDelta, float _cutTimer);
		void SelectAnimation(int _sceneIndex);
		void SetPlayDirection(bool _isRight);

		void SetAlpha(float _alpha);

		virtual void Render(GameObject* _owner);
		
		float GetAlpha();
	};
}