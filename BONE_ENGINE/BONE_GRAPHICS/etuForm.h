#pragma once
#include "Common.h"

namespace BONE_GRAPHICS
{
	class etuForm abstract
	{
	protected:
		LPD3DXSPRITE m_pSprite;
		D3DXVECTOR3 m_vec3Position;
		LPDIRECT3DTEXTURE9 m_pTextures;
		LPD3DXFONT m_pFont;
		std::string m_sName;
		D3DXVECTOR3 m_vec3ParentPosition;

		D3DXMATRIX m_TransformMatrix;
		RECT m_rImageRect;
		RECT m_rMargin;

		int m_iWidth;
		int m_iHeight;

		float m_fScaleX;
		float m_fScaleY;

		bool Is_Clicked;
		bool Is_Focused;
		bool Is_Show;

	public:
		bool BasicSetInformation(std::string p_sName, D3DXVECTOR3 p_vec3Position, int p_iWidth, int p_iHeight, RECT* p_pMargin = nullptr);
		D3DXVECTOR3 GetPosition();
		SIZE GetStaticObjectSize();
		std::string GetName();
		virtual void SetPosition(D3DXVECTOR3& p_vec3Position);
		void SetParentPosition(D3DXVECTOR3 p_vec3Position);
		void SetSize(int p_iWidth, int p_iHeight);
		void SetMargin(RECT* p_pMargin);
		void IsShow(bool p_bShow);
		bool IsShow();
		virtual bool IsClicked();
		bool SetFont(int p_iSize, const char* p_sFont, int p_iSetting);
		virtual void Render() = 0;
		virtual void Release();
	};
}