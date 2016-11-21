#pragma once
#include "Common.h"
#include "etuForm.h"

namespace BONE_GRAPHICS
{
	class etuTextBox : public etuForm
	{
	private:
		string m_sString;

	public:
		bool SetInformaition(string _Name, D3DXVECTOR3 _vec3Position, int _Width, int _Height, string _Text, RECT* _Rect = NULL, RECT* _Margin = NULL);
		void SetText(string _Text);
		virtual void Render() override;
	};
}