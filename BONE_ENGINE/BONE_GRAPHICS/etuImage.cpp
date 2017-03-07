#include "Common.h"
#include "etuImage.h"

namespace BONE_GRAPHICS
{
	bool etuImage::SetInformaition(string p_sName, string p_sImageAddress, D3DXVECTOR3 p_vec3Position, int p_iWidth, int p_iHeight, RECT* p_pRect, RECT* p_pMargin)
	{
		m_rImageRect = *p_pRect;

		if (!BasicSetInformation(p_sName, p_vec3Position, p_iWidth, p_iHeight, p_pMargin))
			return false;

		m_pTextures->Release();
		m_pTextures = nullptr;

		if (FAILED(D3DXCreateTextureFromFileEx(RenderMgr->GetDevice(), p_sImageAddress.c_str(), D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2,
			1, nullptr, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, D3DX_DEFAULT, nullptr, nullptr, nullptr, nullptr, &m_pTextures)))
		{
			return false;
		}

		Is_Show = true;

		return true;
	}

	void etuImage::SetRotation(float _rotAngle)
	{
		rotAngle = _rotAngle;
	}

	void etuImage::Render()
	{
		D3DXMatrixIdentity(&m_TransformMatrix);
		D3DXMatrixTransformation2D(&m_TransformMatrix, &D3DXVECTOR2(m_vec3Position.x, m_vec3Position.y), nullptr, &D3DXVECTOR2(m_fScaleX, m_fScaleY), nullptr, rotAngle, &D3DXVECTOR2(0, 0));

		m_pSprite->SetTransform(&m_TransformMatrix);

		m_pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);

		m_pSprite->Draw(m_pTextures, &m_rImageRect, nullptr, &(m_vec3ParentPosition + m_vec3Position), 0xffffffff);

		m_pSprite->End();
	}

	void etuImage::SetImageRect(RECT rect)
	{
		m_rImageRect = rect;
	}

	void etuImage::Render(int Alpha)
	{
		D3DXMatrixIdentity(&m_TransformMatrix);
		D3DXMatrixTransformation2D(&m_TransformMatrix, nullptr, nullptr, &D3DXVECTOR2(m_fScaleX, m_fScaleY), &D3DXVECTOR2(m_vec3Position.x + 42, m_vec3Position.y + 46), rotAngle, nullptr);

		m_pSprite->SetTransform(&m_TransformMatrix);

		m_pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);

		D3DCOLOR RGB = D3DCOLOR_ARGB(Alpha, 255, 255, 255);
		m_pSprite->Draw(m_pTextures, &m_rImageRect, nullptr, &(m_vec3ParentPosition + m_vec3Position), RGB);

		m_pSprite->End();
	}
}