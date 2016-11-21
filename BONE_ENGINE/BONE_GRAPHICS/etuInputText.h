#pragma once
#include "Common.h"
#include "etuForm.h"

namespace BONE_GRAPHICS
{
	class etuInputText : public etuForm
	{
	private:
		RECT m_rFocusRect;
		string m_sString;

	public:
		bool SetInformaition(string _Name, D3DXVECTOR3 _vec3Position, int _Width, string _Text);
		virtual bool IsClicked();
		void Update(float _fTimeDelta);
		string GetText();
		virtual void Render() override;
	};
}