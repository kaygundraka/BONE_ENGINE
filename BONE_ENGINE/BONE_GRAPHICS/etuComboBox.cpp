#include "Common.h"
#include "etuComboBox.h"
#include "InputManager.h"

namespace BONE_GRAPHICS
{
	bool etuComboBox::SetInformaition(std::string _Name, D3DXVECTOR3 _vec3Position)
	{
		m_sName = _Name;
		m_sSelectItem = "NONE";

		m_rImageRect	= { 334,  50, 488,  85, };
		m_rClicked		= { 334, 105, 488, 135, };
		m_rItem			= { 334, 165, 488, 194, };
		m_rSelectItem	= { 334, 193, 488, 220, };
		m_rItemLast		= { 334, 247, 488, 275, };

		m_pItems.push_back(m_sSelectItem);

		m_rItemHeight = m_rItem.bottom - m_rItem.top;
		
		if (!BasicSetInformation(p_sName, _vec3Position, 154, 35, nullptr))
			return false;

		Is_Show = true;
		Is_SelectStatus = false;

		return true;
	}

	bool etuComboBox::AddItems(std::string _String)
	{
		for (int i = 0; i < m_pItems.size(); i++)
		{
			if (m_pItems[i] == _String)
				return false;
		}

		m_pItems.push_back(p_sString);

		return true;
	}

	bool etuComboBox::DeleteItems(std::string _String)
	{
		for (auto Iter = m_pItems.begin(); Iter < m_pItems.end(); Iter++)
		{
			if (Iter->compare(p_sString))
			{
				Iter = m_pItems.erase(Iter);
				return true;
			}
		}

		return false;
	}

	std::string etuComboBox::GetSelectItems()
	{
		return m_sSelectItem;
	}

	bool etuComboBox::IsClicked()
	{
		POINT Temp = InputMgr->GetMousePosition();

		POINT RectNormal;
		RectNormal.x = m_vec3ParentPosition.x + m_vec3Position.x;
		RectNormal.y = m_vec3ParentPosition.y + m_vec3Position.y;

		if (Is_SelectStatus)
		{
			if (Temp.x <= RectNormal.x + m_iWidth && Temp.x >= RectNormal.x)
			{
				if (Temp.y <= RectNormal.y + m_iHeight + m_pItems.size() * m_rItemHeight && Temp.y >= RectNormal.y)
				{
					if (InputMgr->GetMouseLBButtonStatus() == MOUSE_LBDOWN)
					{
						for (int i = 1; i < m_pItems.size(); i++)
						{
							if (Temp.y <= RectNormal.y + m_iHeight + ((i + 2) * m_rItemHeight ) && Temp.y >= RectNormal.y + ((i + 1) * m_rItemHeight))
							{
								m_sSelectItem = m_pItems[i];

								Is_SelectStatus = false;
								Is_Clicked = false;
							}
						}
					}

					return true;
				}
			}
		}
		
		if (InputMgr->GetMouseLBButtonStatus() == MOUSE_LBDOWN)
		{
			if (Temp.x <= RectNormal.x + m_iWidth && Temp.x >= RectNormal.x + 134)
			{
				if (Temp.y <= RectNormal.y + m_iHeight && Temp.y >= RectNormal.y)
				{
					Is_Clicked = true;
					Is_SelectStatus = true;

					return true;
				}
			}
		}

		Is_SelectStatus = false;
		Is_Clicked = false;

		return false;
	}

	void etuComboBox::Render()
	{
		if (Is_Show)
		{
			D3DXMatrixIdentity(&m_TransformMatrix);
			D3DXMatrixTransformation2D(&m_TransformMatrix, &D3DXVECTOR2(m_vec3Position.x + m_vec3ParentPosition.x, m_vec3Position.y + m_vec3ParentPosition.y),
				nullptr, &D3DXVECTOR2(m_fScaleX, m_fScaleY), nullptr, 0.0f, nullptr);

			m_pSprite->SetTransform(&m_TransformMatrix);

			m_pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);

			if (Is_Clicked)
			{
				m_pSprite->Draw(m_pTextures, &m_rClicked, nullptr, &(m_vec3ParentPosition + m_vec3Position), 0xffffffff);

				D3DXVECTOR3 ItemsPosition(0, 0, 0);

				for (int i = 0; i < m_pItems.size(); i++)
				{
					ItemsPosition.y += m_rItemHeight;

					if (m_pItems.size() - i == 1)
					{
						m_pSprite->Draw(m_pTextures, &m_rItemLast, nullptr, &(m_vec3ParentPosition + m_vec3Position + ItemsPosition), 0xffffffff);
					}
					else
					{
						m_pSprite->Draw(m_pTextures, &m_rItem, nullptr, &(m_vec3ParentPosition + m_vec3Position + ItemsPosition), 0xffffffff);
					}
				}
			}
			else
				m_pSprite->Draw(m_pTextures, &m_rImageRect, nullptr, &(m_vec3ParentPosition + m_vec3Position), 0xffffffff);

			m_pSprite->End();

			SIZE SelectItemSize;

			GetTextExtentPoint32(m_pFont->GetDC(), m_sSelectItem.c_str(), m_sSelectItem.length(), &SelectItemSize);

			RECT SelectItemTextPosition = {
				m_vec3ParentPosition.x + m_vec3Position.x + m_iWidth * m_fScaleX / 2 - SelectItemSize.cx / 2,
				m_vec3ParentPosition.y + m_vec3Position.y + SelectItemSize.cy / 2,
				m_vec3ParentPosition.x + m_vec3Position.x + m_iWidth * m_fScaleX,
				m_vec3ParentPosition.y + m_vec3Position.y + m_iHeight * m_fScaleY + SelectItemSize.cy / 2,
			};

			m_pFont->DrawText(nullptr, m_sSelectItem.c_str(), -1, &SelectItemTextPosition, 0, 0xFFFFFFFF);


			if (Is_Clicked)
			{
				D3DXVECTOR3 ItemsPosition(0, 0, 0);

				for (int i = 0; i < m_pItems.size(); i++)
				{
					SIZE ItemTextSize;

					ItemsPosition.y += m_rItemHeight;

					GetTextExtentPoint32(m_pFont->GetDC(), m_pItems[i].c_str(), m_pItems[i].length(), &ItemTextSize);

					RECT ItemTextPosition = {
						m_vec3ParentPosition.x + m_vec3Position.x + m_iWidth * m_fScaleX / 2 - ItemTextSize.cx / 2,
						m_vec3ParentPosition.y + m_vec3Position.y + ItemsPosition.y + ItemTextSize.cy / 2,
						m_vec3ParentPosition.x + m_vec3Position.x + m_iWidth * m_fScaleX,
						m_vec3ParentPosition.y + m_vec3Position.y + ItemsPosition.y + m_iHeight * m_fScaleY + ItemTextSize.cy / 2,
					};

					m_pFont->DrawText(nullptr, m_pItems[i].c_str(), -1, &ItemTextPosition, 0, 0xFFFFFFFF);
				}
			}
		}
	}
}