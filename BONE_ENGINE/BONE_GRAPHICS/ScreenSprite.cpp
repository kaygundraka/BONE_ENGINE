#include "Common.h"
#include "ScreenSprite.h"
#include "Transform2D.h"
#include "ResourceManager.h"

namespace BONE_GRAPHICS
{
	ScreenSprite::ScreenSprite()
	{
		ThreadSync sync;

		SetTypeName("ScreenSprite");

		D3DXCreateSprite(RenderMgr->GetDevice(), &sprite);
	}

	ScreenSprite::~ScreenSprite()
	{
		ThreadSync sync;

		sprite->Release();
	}

	void ScreenSprite::SetInformation(string _address, int _width, int _height, int _animationCut, int _animationScene, float _alpha)
	{
		ThreadSync sync;

		width = _width;
		height = _height;
		animeCut = _animationCut;
		animeScene = _animationScene;

		address = _address;

		RECT rect = {
			0, 0, width / animeCut, height / animeScene,
		};

		RECT Margin = { 0, 0, 0, 0 };

		curAnimeCut = 0;
		curAnimeScene = 0;
		alpha = 255;

		rightPlay = true;
	}
	
	void ScreenSprite::SetPlayDirection(bool _isRight)
	{
		rightPlay = _isRight;
	}

	void ScreenSprite::PlayAnimation(float _timeDelta, float _cutTiemr)
	{
		ThreadSync sync;

		RECT rect = {
			width / animeCut * curAnimeCut, height / animeScene * curAnimeScene,
			width / animeCut * (curAnimeCut + 1), height / animeScene * (curAnimeScene + 1)
		};

		curRect = rect;

		static float CutTimer = 0;
		
		if (CutTimer > _cutTiemr)
		{
			if (rightPlay)
			{
				if (curAnimeCut >= animeCut - 1)
					curAnimeCut = 0;
				else
					curAnimeCut++;
			}
			else
			{
				if (curAnimeCut <= 0)
					curAnimeCut = animeCut - 1;
				else
					curAnimeCut--;
			}

			CutTimer = 0;
		}
		else
			CutTimer += _timeDelta;
	}

	void ScreenSprite::Render(GameObject* _owner)
	{
		ThreadSync sync;

		Matrix matrix = ((Transform2D*)_owner->GetComponent("Transform2D"))->GetTransform();
		Vector3 position = ((Transform2D*)_owner->GetComponent("Transform2D"))->GetPosition();

		LPDIRECT3DTEXTURE9 texture = ResourceMgr->LoadTexture(address);

		sprite->SetTransform(&matrix);

		sprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);

		sprite->Draw(texture, &curRect, nullptr, &Vector3(0, 0, 0), 0xffffffff);

		sprite->End();
	}

	void ScreenSprite::LoadContent()
	{
		ThreadSync sync;

		ResourceMgr->LoadTexture(address);
	}

	void ScreenSprite::SelectAnimation(int _sceneIndex)
	{
		ThreadSync sync;

		if (curAnimeScene != _sceneIndex)
		{
			curAnimeScene = _sceneIndex;
			curAnimeCut = 0;
		}
	}

	void ScreenSprite::SetAlpha(float _alpha)
	{
		ThreadSync sync;

		alpha = _alpha;
	}

	float ScreenSprite::GetAlpha()
	{
		ThreadSync sync;

		return alpha;
	}
}