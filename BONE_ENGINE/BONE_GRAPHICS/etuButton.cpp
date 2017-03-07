#include "Common.h"
#include "etuButton.h"
#include "RenderManager.h"
#include "InputManager.h"
#pragma warning(disable:4996)

namespace BONE_GRAPHICS
{
	bool etuButton::SetInformaition(string _Name, D3DXVECTOR3 _vec3Position, int _Width, int _Height, string _Text, RECT* _Rect, RECT* _Margin)
	{
		m_rImageRect = { 213, 499, 357, 535, };
		m_rClickDown = { 213, 555, 357, 590, };

		m_sString = _Text;

		if (!BasicSetInformation(p_sName, _vec3Position, _Width, _Height, _Margin))
			return false;

		Is_Show = true;

		return true;
	}

	void etuButton::SetText(string _Text)
	{
		m_sString = _Text;
	}

	bool etuButton::IsClicked()
	{
		if (InputMgr->GetMouseLBButtonStatus() == MOUSE_LBDOWN)
		{
			POINT Temp = InputMgr->GetMousePosition();

			if (Temp.x <= m_vec3ParentPosition.x + m_vec3Position.x + m_iWidth * m_fScaleX
				&& Temp.x >= m_vec3Position.x + m_vec3ParentPosition.x)
			{
				if (Temp.y <= m_vec3ParentPosition.y + m_vec3Position.y + m_iHeight * m_fScaleY
					&& Temp.y >= m_vec3Position.y + m_vec3ParentPosition.y)
				{
					if (Is_Clicked)
						return true;

					Is_Clicked = true;

					return true;
				}
			}
		}
		else
		{
			Is_Clicked = false;
		}

		return false;
	}

	void etuButton::Render()
	{
		if (Is_Show)
		{
			D3DXMatrixIdentity(&m_TransformMatrix);
			D3DXMatrixTransformation2D(&m_TransformMatrix, &D3DXVECTOR2(m_vec3Position.x + m_vec3ParentPosition.x, m_vec3Position.y + m_vec3ParentPosition.y),
				nullptr, &D3DXVECTOR2(m_fScaleX, m_fScaleY), nullptr, 0.0f, nullptr);

			m_pSprite->SetTransform(&m_TransformMatrix);

			m_pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);

			if (Is_Clicked)
				m_pSprite->Draw(m_pTextures, &m_rClickDown, nullptr, &(m_vec3ParentPosition + m_vec3Position), 0xffffffff);
			else
				m_pSprite->Draw(m_pTextures, &m_rImageRect, nullptr, &(m_vec3ParentPosition + m_vec3Position), 0xffffffff);

			m_pSprite->End();

			RECT TextRect = {
				m_vec3ParentPosition.x + m_vec3Position.x + m_rMargin.left,
				m_vec3ParentPosition.y + m_vec3Position.y + m_rMargin.top,
				m_vec3ParentPosition.x + m_vec3Position.x + m_iWidth * m_fScaleX - m_rMargin.right,
				m_vec3ParentPosition.y + m_vec3Position.y + m_iHeight * m_fScaleY - m_rMargin.bottom,
			};

			static SIZE size;

			GetTextExtentPoint32(m_pFont->GetDC(), m_sString.c_str(), m_sString.length(), &size);

			RECT TextPosition = {
				m_vec3ParentPosition.x + m_vec3Position.x + m_iWidth * m_fScaleX / 2 - size.cx / 2,
				m_vec3ParentPosition.y + m_vec3Position.y + m_iHeight * m_fScaleY / 2 - size.cy / 2 ,
				m_vec3ParentPosition.x + m_vec3Position.x + m_iWidth * m_fScaleX,
				m_vec3ParentPosition.y + m_vec3Position.y + m_iHeight * m_fScaleY ,
			};

			m_pFont->DrawText(nullptr, m_sString.c_str(), -1, &TextPosition, 0, 0xFFFFFFFF);

		}
	}
}