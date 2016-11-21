#include "Common.h"
#include "etuTextBox.h"
#include "RenderManager.h"
#pragma warning(disable:4996)

namespace BONE_GRAPHICS
{
	bool etuTextBox::SetInformaition(string _Name, D3DXVECTOR3 _vec3Position, int _Width, int _Height, string _Text, RECT* _Rect, RECT* _Margin)
	{
		if (p_pRect == NULL)
			m_rImageRect = { 527, 160, 833, 273, };
		else
			m_rImageRect = *p_pRect;

		m_sString = _Text;

		if (!BasicSetInformation(p_sName, _vec3Position, _Width, _Height, _Margin))
			return false;

		Is_Show = true;

		return true;
	}

	void etuTextBox::SetText(string _Text)
	{
		m_sString = _Text;
	}

	void etuTextBox::Render()
	{
		if (Is_Show)
		{
			D3DXMatrixIdentity(&m_TransformMatrix);
			D3DXMatrixTransformation2D(&m_TransformMatrix, &D3DXVECTOR2(m_vec3Position.x + m_vec3ParentPosition.x, m_vec3Position.y + m_vec3ParentPosition.y),
				NULL, &D3DXVECTOR2(m_fScaleX, m_fScaleY), NULL, 0.0f, &D3DXVECTOR2(0, 0));

			m_pSprite->SetTransform(&m_TransformMatrix);

			m_pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);
			m_pSprite->Draw(m_pTextures, &m_rImageRect, NULL, &(m_vec3ParentPosition + m_vec3Position), 0xffffffff);
			m_pSprite->End();

			RECT TextRect = {
				m_vec3ParentPosition.x + m_vec3Position.x + m_rMargin.left,
				m_vec3ParentPosition.y + m_vec3Position.y + m_rMargin.top,
				m_vec3ParentPosition.x + m_vec3Position.x + m_iWidth * m_fScaleX - m_rMargin.right,
				m_vec3ParentPosition.y + m_vec3Position.y + m_iHeight * m_fScaleY - m_rMargin.bottom,
			};

			string Text = m_sString;
			SIZE TextSize;
			GetTextExtentPoint32(m_pFont->GetDC(), m_sString.c_str(), m_sString.length(), &TextSize);

			int Line = TextSize.cx / (TextRect.right - TextRect.left);

			for (int i = 1; i <= Line; i++)
			{
				int index = Text.length() / Line;
				Text.insert(index * i, "\n");
			}

			m_pFont->DrawText(NULL, Text.c_str(), -1, &TextRect, 0, 0xFFFFFFFF);

		}
	}
}