#include "Common.h"
#include "etuInputText.h"
#include "RenderManager.h"
#include "InputManager.h"
#pragma warning(disable:4996)

namespace BONE_GRAPHICS
{
	bool etuInputText::SetInformaition(string _Name, D3DXVECTOR3 _vec3Position, int _Width, string _Text)
	{
		m_rImageRect = { 50, 50, 296, 86, };
		m_rFocusRect = { 50, 192, 296, 229, };
			
		m_sString = _Text;

		if (!BasicSetInformation(p_sName, _vec3Position, _Width, m_rImageRect.bottom - m_rImageRect.top, nullptr))
			return false;

		Is_Show = true;
		Is_Focused = false;

		return true;
	}

	bool etuInputText::IsClicked()
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
					if (!Is_Clicked)
						InputMgr->SetFocusTextInput(1);

					Is_Clicked = true;
					Is_Focused = true;

					return true;
				}
			}

			if(Is_Focused)
				InputMgr->SetFocusTextInput(-1);

			Is_Focused = false;
		}
		else
		{
			Is_Clicked = false;
		}

		return false;
	}

	string etuInputText::GetText()
	{
		return m_sString;
	}

	void etuInputText::Update(float _fTimeDelta)
	{
		if (Is_Focused)
		{
			static float KeyDown = 0;

			if ((GetAsyncKeyState(VK_BACK) & 0x8000) && KeyDown >= 0.1f)
			{
				char Text[100];				
				strcpy_s(Text, m_sString.c_str());

				if(strlen(Text) >= 1)
					Text[strlen(Text) - 1] = 0;

				m_sString = Text;
				KeyDown = 0;
			}
			else
			{
				char Temp[2];
				Temp[0] = InputMgr->GetCurrentChar();
				Temp[1] = '\0';

				if (Temp[0] >= 32 && Temp[0] <= 126)
				{
					char Text[100];
					strcpy_s(Text, m_sString.c_str());
					strcat_s(Text, Temp);

					m_sString = Text;
				}
			}

			if (KeyDown <= 0.1f)
				KeyDown += _fTimeDelta;
		}
	}

	void etuInputText::Render()
	{
		if (Is_Show)
		{
			D3DXMatrixIdentity(&m_TransformMatrix);
			D3DXMatrixTransformation2D(&m_TransformMatrix, &D3DXVECTOR2(m_vec3Position.x + m_vec3ParentPosition.x, m_vec3Position.y + m_vec3ParentPosition.y),
				nullptr, &D3DXVECTOR2(m_fScaleX, m_fScaleY), nullptr, 0.0f, &D3DXVECTOR2(0, 0));

			m_pSprite->SetTransform(&m_TransformMatrix);

			m_pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);
			
			if(Is_Focused)
				m_pSprite->Draw(m_pTextures, &m_rFocusRect, nullptr, &(m_vec3ParentPosition + m_vec3Position), 0xffffffff);
			else
				m_pSprite->Draw(m_pTextures, &m_rImageRect, nullptr, &(m_vec3ParentPosition + m_vec3Position), 0xffffffff);
			
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

			m_pFont->DrawText(nullptr, Text.c_str(), -1, &TextRect, 0, 0xFFFFFFFF);
		}
	}
}