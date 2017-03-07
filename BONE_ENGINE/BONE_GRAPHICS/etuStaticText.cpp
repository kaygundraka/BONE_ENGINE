#include "Common.h"
#include "etuStaticText.h"
#include "RenderManager.h"
#pragma warning(disable:4996)

namespace BONE_GRAPHICS
{
	bool etuStaticText::SetInformaition(string _Name, D3DXVECTOR3 _vec3Position, int _Width, int _Height, string _Text)
	{
		m_sString = _Text;

		if (!BasicSetInformation(p_sName, _vec3Position, _Width, _Height, nullptr))
			return false;

		m_iWidth = _Width;
		m_iHeight = _Height;

		Is_Show = true;

		return true;
	}

	void etuStaticText::SetText(string _Text)
	{
		m_sString = _Text;
	}

	void etuStaticText::Render()
	{
		if (Is_Show)
		{
			RECT TextRect = {
				m_vec3ParentPosition.x + m_vec3Position.x,
				m_vec3ParentPosition.y + m_vec3Position.y,
				m_vec3ParentPosition.x + m_vec3Position.x + m_iWidth,
				m_vec3ParentPosition.y + m_vec3Position.y + m_iHeight,
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

			m_pFont->DrawText(nullptr, Text.c_str(), -1, &TextRect, 0, 0xFFFFFFFF);

		}
	}
}