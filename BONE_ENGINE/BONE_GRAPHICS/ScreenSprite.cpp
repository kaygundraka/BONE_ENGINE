#include "Common.h"
#include "ScreenSprite.h"
#include "Transform2D.h"
#include "ResourceManager.h"
#include "SceneManager.h"

namespace BONE_GRAPHICS
{
	ScreenSprite::ScreenSprite()
	{
		SetTypeName("ScreenSprite");

		D3DXCreateSprite(RenderMgr->GetDevice(), &sprite);
	}

	ScreenSprite::~ScreenSprite()
	{
		sprite->Release();
	}

	void ScreenSprite::SetInformation(string address, int width, int height, int animationCut, int animationScene, float alpha)
	{
		this->width = width;
        this->height = height;
		animeCut = animationCut;
		animeScene = animationScene;

        this->address = address;

		RECT rect = {
			0, 0, width / animeCut, height / animeScene,
		};

		RECT Margin = { 0, 0, 0, 0 };

		curAnimeCut = 0;
		curAnimeScene = 0;
		alpha = 255;

		rightPlay = true;
	}
	
	void ScreenSprite::SetPlayDirection(bool isRight)
	{
		rightPlay = isRight;
	}

	void ScreenSprite::PlayAnimation(float cutTiemr)
	{
		RECT rect = {
			width / animeCut * curAnimeCut, height / animeScene * curAnimeScene,
			width / animeCut * (curAnimeCut + 1), height / animeScene * (curAnimeScene + 1)
		};

		curRect = rect;

		static float CutTimer = 0;
		
		if (CutTimer > cutTiemr)
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
			CutTimer += SceneMgr->GetTimeDelta();
	}

	void ScreenSprite::Render(GameObject* owner)
	{
		Matrix matrix = ((Transform2D*)owner->GetComponent("Transform2D"))->GetTransform();
		Vector3 position = ((Transform2D*)owner->GetComponent("Transform2D"))->GetPosition();

		LPDIRECT3DTEXTURE9 texture = ResourceMgr->LoadTexture(address);

		sprite->SetTransform(&matrix);

		sprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);

		sprite->Draw(texture, &curRect, nullptr, &Vector3(0, 0, 0), 0xffffffff);

		sprite->End();
	}

	void ScreenSprite::LoadContent()
	{
		ResourceMgr->LoadTexture(address);
	}

	void ScreenSprite::SelectAnimation(int sceneIndex)
	{
		if (curAnimeScene != sceneIndex)
		{
			curAnimeScene = sceneIndex;
			curAnimeCut = 0;
		}
	}

	void ScreenSprite::SetAlpha(float alpha)
	{
		this->alpha = alpha;
	}

	float ScreenSprite::GetAlpha()
	{
		return alpha;
	}
}