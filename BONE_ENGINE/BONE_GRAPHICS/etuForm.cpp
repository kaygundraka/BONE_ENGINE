#include "Common.h"
#include "etuForm.h"
#include "RenderManager.h"
#include "InputManager.h"

namespace BONE_GRAPHICS
{
	bool etuForm::BasicSetInformation(string p_sName, D3DXVECTOR3 p_vec3Position, int p_iWidth, int p_iHeight, RECT* p_pMargin)
	{
		m_sName = p_sName;

		if (FAILED(D3DXCreateTextureFromFileEx(RenderMgr->GetDevice(), "Resource\\GUI\\GUI.png", D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2,
			1, nullptr, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, D3DX_DEFAULT, nullptr, nullptr, nullptr, nullptr, &m_pTextures)))
		{
			return false;
		}

		if (FAILED(D3DXCreateSprite(RenderMgr->GetDevice(), &m_pSprite)))
		{
			return false;
		}

		m_vec3Position = p_vec3Position;

		m_iWidth = m_rImageRect.right - m_rImageRect.left;
		m_iHeight = m_rImageRect.bottom - m_rImageRect.top;

		if (m_vec3Position.z > 1)
			m_vec3Position.z = 1;
		else if (m_vec3Position < 0)
			m_vec3Position.z = 0;

		if (p_pMargin != nullptr)
		{
			m_rMargin = {
				(int)p_pMargin->left,
				(int)p_pMargin->top,
				(int)p_pMargin->right,
				(int)p_pMargin->bottom,
			};
		}
		else
		{
			m_rMargin = {
				10,
				10,
				10,
				10,
			};
		}

		if (FAILED(D3DXCreateFont(RenderMgr->GetDevice(), 20, 0, nullptr, 0, FALSE, DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &m_pFont)))
		{
			return false;
		}

		m_fScaleX = (float)p_iWidth / (float)m_iWidth;
		m_fScaleY = (float)p_iHeight / (float)m_iHeight;

		m_vec3ParentPosition = D3DXVECTOR3(0, 0, 0);

		Is_Show = true;
		Is_Clicked = false;

		return true;
	}

	bool etuForm::IsShow()
	{
		return Is_Show;
	}

	D3DXVECTOR3 etuForm::GetPosition()
	{
		return m_vec3Position + m_vec3ParentPosition;
	}

	SIZE etuForm::GetStaticObjectSize()
	{
		SIZE Temp;
		Temp.cx = m_iWidth;
		Temp.cy = m_iHeight;

		return Temp;
	}

	string etuForm::GetName()
	{
		return m_sName;
	}

	void etuForm::SetMargin(RECT* p_pMargin)
	{
		if (p_pMargin != nullptr)
		{
			m_rMargin = {
				(int)m_vec3Position.x + p_pMargin->left,
				(int)m_vec3Position.y + p_pMargin->top,
				(int)m_vec3Position.x + m_iWidth - p_pMargin->right,
				(int)m_vec3Position.y + m_iHeight - p_pMargin->bottom,
			};
		}
		else
		{
			m_rMargin = {
				(int)m_vec3Position.x + 10,
				(int)m_vec3Position.y + 10,
				(int)m_vec3Position.x + m_iWidth - 10,
				(int)m_vec3Position.y + m_iHeight - 10,
			};
		}
	}

	void etuForm::SetSize(int p_iWidth, int p_iHeight)
	{
		m_fScaleX = (float)p_iWidth / (float)m_iWidth;
		m_fScaleY = (float)p_iHeight / (float)m_iHeight;
	}

	void etuForm::SetParentPosition(D3DXVECTOR3 p_vec3Position)
	{
		m_vec3ParentPosition = p_vec3Position;
	}

	void etuForm::IsShow(bool p_bShow)
	{
		Is_Show = p_bShow;
	}

	bool etuForm::SetFont(int p_iSize, const char* p_sFont, int p_iSetting)
	{
		m_pFont->Release();
		m_pFont = nullptr;

		if (FAILED(D3DXCreateFont(RenderMgr->GetDevice(), p_iSize, 0, p_iSetting, 0, FALSE, DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, p_sFont, &m_pFont)))
		{
			return false;
		}
	}

	bool etuForm::IsClicked()
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

	void etuForm::SetPosition(D3DXVECTOR3& p_vec3Position)
	{
		m_vec3Position = p_vec3Position;
	}

	void etuForm::Release()
	{
		if (!m_pSprite)
			m_pSprite->Release();

		if (!m_pTextures)
			m_pTextures->Release();

		if (!m_pFont)
			m_pFont->Release();
	}
}