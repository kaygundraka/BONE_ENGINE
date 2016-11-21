#include "Common.h"
#include "etuCheckBox.h"
#include "RenderManager.h"
#include "InputManager.h"
#pragma warning(disable:4996)

namespace BONE_GRAPHICS
{
	bool etuCheckBox::SetInformaition(string _Name, D3DXVECTOR3 _vec3Position, string _Text, RECT* _Rect, RECT* _Margin)
	{
		m_rImageRect = { 50, 294, 65, 308, };
		m_rCheck = { 50, 328, 65, 343, };

		m_sString = _Text;

		if (!BasicSetInformation(p_sName, _vec3Position, 15, 15, _Margin))
			return false;

		Is_Show = true;
		Is_Checked = false;

		return true;
	}

	bool etuCheckBox::GetStatus()
	{
		return Is_Checked;
	}

	void etuCheckBox::SetStatus(bool Is_Checked)
	{
		this->Is_Checked = Is_Checked;
	}

	void etuCheckBox::SetText(string _Text)
	{
		m_sString = _Text;
	}

	bool etuCheckBox::IsClicked()
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

					if (Is_Checked)
						Is_Checked = false;
					else
						Is_Checked = true;

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

	void etuCheckBox::Render()
	{
		if (Is_Show)
		{
			D3DXMatrixIdentity(&m_TransformMatrix);
			D3DXMatrixTransformation2D(&m_TransformMatrix, &D3DXVECTOR2(m_vec3Position.x + m_vec3ParentPosition.x, m_vec3Position.y + m_vec3ParentPosition.y),
				NULL, &D3DXVECTOR2(m_fScaleX, m_fScaleY), NULL, 0.0f, NULL);

			m_pSprite->SetTransform(&m_TransformMatrix);

			m_pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);

			if (Is_Checked)
				m_pSprite->Draw(m_pTextures, &m_rCheck, NULL, &(m_vec3ParentPosition + m_vec3Position), 0xffffffff);
			else
				m_pSprite->Draw(m_pTextures, &m_rImageRect, NULL, &(m_vec3ParentPosition + m_vec3Position), 0xffffffff);

			m_pSprite->End();

			static SIZE size;

			GetTextExtentPoint32(m_pFont->GetDC(), m_sString.c_str(), m_sString.length(), &size);

			RECT TextPosition = {
				m_vec3ParentPosition.x + m_vec3Position.x + m_iWidth * m_fScaleX + 5,
				m_vec3ParentPosition.y + m_vec3Position.y,
				m_vec3ParentPosition.x + m_vec3Position.x + m_iWidth * m_fScaleX + size.cx,
				m_vec3ParentPosition.y + m_vec3Position.y + m_iHeight * m_fScaleY,
			};

			m_pFont->DrawText(NULL, m_sString.c_str(), -1, &TextPosition, 0, 0xFFFFFFFF);

		}
	}
}