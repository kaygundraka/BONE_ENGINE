#include "Common.h"
#include "ScreenButton.h"
#include "InputManager.h"

namespace BONE_GRAPHICS
{
	ScreenButton::ScreenButton()
	{
		CThreadSync sync;

		SetTypeName("ScreenButton");

		status = BUTTON_STATUS::NORMAL;

		D3DXCreateSprite(RenderMgr->GetDevice(), &sprite);
	}

	ScreenButton::~ScreenButton()
	{
		CThreadSync sync;

		sprite->Release();
	}

	void ScreenButton::LoadContent()
	{
		CThreadSync sync;

		ResourceMgr->LoadTexture(address);
	}

	ScreenButton::BUTTON_STATUS ScreenButton::Update(GameObject* _owner, float _timeDelta)
	{
		CThreadSync sync;

		Transform2D* tr = ((Transform2D*)_owner->GetComponent("Transform2D"));

		int width = tr->GetSize().x;
		int height = tr->GetSize().y;

		if (tr == NULL)
			return BUTTON_STATUS::NORMAL;

		Vector2 position(
			tr->GetPosition().x,
			tr->GetPosition().y
		);

		POINT temp = InputMgr->GetMousePosition();

		if (temp.x <= position.x + width && temp.x >= position.x)
		{
			if (temp.y <= position.y + height && temp.y >= position.y)
			{
				if (InputMgr->GetMouseLBButtonStatus() == MOUSE_LBDOWN)
				{
					status = BUTTON_STATUS::PRESSED;
					return status;
				}
				else
				{
					status = BUTTON_STATUS::HIGHLIGHT;
					return status;
				}
			}
		}

		status = BUTTON_STATUS::NORMAL;

		return status;
	}

	void ScreenButton::SetOriginRect(Rect _rect)
	{
		CThreadSync sync;

		originRect = _rect;
	}
	
	void ScreenButton::SetOriginRect(Vector2 _leftTop, Vector2 _rightBottom)
	{
		CThreadSync sync;

		originRect.LeftTop = _leftTop;
		originRect.RightBottom = _rightBottom;
	}

	Rect ScreenButton::GetOriginRect()
	{
		CThreadSync sync;

		return originRect;
	}

	void ScreenButton::SetImageFile(string _address)
	{
		CThreadSync sync;

		address = _address;
	}

	void ScreenButton::SetAlpha(float _alpha)
	{
		CThreadSync sync;

		alpha = _alpha;
	}

	void ScreenButton::Render(GameObject* _owner)
	{
		CThreadSync sync;

		Matrix matrix = ((Transform2D*)_owner->GetComponent("Transform2D"))->GetTransform();
		Vector3 position = ((Transform2D*)_owner->GetComponent("Transform2D"))->GetPosition();

		RECT rect;
		rect.left = originRect.LeftTop.x;
		rect.top = originRect.LeftTop.y;
		rect.right = originRect.RightBottom.x;
		rect.bottom = originRect.RightBottom.y;

		switch (status) {
		case NORMAL:
			rect.left = 0;
			rect.right = rect.right / 3;
			break;

		case HIGHLIGHT:
			rect.left = rect.right / 3;
			rect.right = rect.right / 3 * 2;
			break;

		case PRESSED:
			rect.left = rect.right / 3 * 2;
			rect.right = rect.right;
			break;
		}
		
		LPDIRECT3DTEXTURE9 texture = ResourceMgr->LoadTexture(address);

		sprite->SetTransform(&matrix);

		sprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);

		sprite->Draw(texture, &rect, NULL, &Vector3(0, 0, 0), 0xffffffff);

		sprite->End();
	}

	float ScreenButton::GetAlpha()
	{
		CThreadSync sync;

		return alpha;
	}
}