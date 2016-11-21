#include "Common.h"
#include "ResourceManager.h"
#include "Transform2D.h"
#include "ScreenImage.h"

namespace BONE_GRAPHICS
{
	ScreenImage::ScreenImage()
	{
		CThreadSync sync;

		SetTypeName("ScreenImage");

		alpha = 255;

		D3DXCreateSprite(RenderMgr->GetDevice(), &sprite);
	}
	
	ScreenImage::~ScreenImage()
	{
		CThreadSync sync;

		sprite->Release();
	}

	void ScreenImage::LoadContent()
	{
		CThreadSync sync;

		ResourceMgr->LoadTexture(address);
	}

	void ScreenImage::SetOriginRect(Rect _rect)
	{
		CThreadSync sync;

		originRect = _rect;
	}

	void ScreenImage::SetOriginRect(Vector2 _leftUp, Vector2 _rightBottom)
	{
		CThreadSync sync;

		originRect.LeftTop = _leftUp;
		originRect.RightBottom = _rightBottom;
	}

	Rect ScreenImage::GetOriginRect()
	{
		CThreadSync sync;

		return originRect;
	}

	void ScreenImage::SetImageFile(string _address)
	{
		CThreadSync sync;

		address = _address;
	}

	void ScreenImage::SetAlpha(float _alpha)
	{
		CThreadSync sync;

		alpha = _alpha;
	}
	
	void ScreenImage::Render(GameObject* _owner)
	{
		CThreadSync sync;

		Matrix matrix = ((Transform2D*)_owner->GetComponent("Transform2D"))->GetTransform();
		Vector3 position = ((Transform2D*)_owner->GetComponent("Transform2D"))->GetPosition();

		RECT rect;
		rect.left = originRect.LeftTop.x;
		rect.top = originRect.LeftTop.y;
		rect.right = originRect.RightBottom.x;
		rect.bottom = originRect.RightBottom.y;

		LPDIRECT3DTEXTURE9 texture = ResourceMgr->LoadTexture(address);

		sprite->SetTransform(&matrix);

		RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

		// alpha - normal 
		RenderMgr->GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		RenderMgr->GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		RenderMgr->GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		sprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);
		sprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_OBJECTSPACE);   //3D¿ë

		D3DCOLOR RGB = D3DCOLOR_ARGB((int)alpha, 255, 255, 255);
		sprite->Draw(texture, &rect, NULL, &Vector3(0, 0, 0), RGB);
		sprite->End();

		RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	}
	
	float ScreenImage::GetAlpha()
	{
		CThreadSync sync;

		return alpha;
	}
}