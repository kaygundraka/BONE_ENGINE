#pragma once
#include "Common.h"
#include "etuForm.h"

namespace BONE_GRAPHICS
{
	class etuButton : public etuForm
	{
	private:
		string m_sString;
		RECT m_rClickDown;

	public:
		bool SetInformaition(string _Name, D3DXVECTOR3 _vec3Position, int _Width, int _Height, string _Text, RECT* _Rect = nullptr, RECT* _Margin = nullptr);
		void SetText(string _Text);
		virtual bool IsClicked() override;
		virtual void Render() override;
	};
}