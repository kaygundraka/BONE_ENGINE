#pragma once
#include "Common.h"
#include "etuForm.h"

namespace BONE_GRAPHICS
{
	class etuCheckBox : public etuForm
	{
	private:
		std::string m_sString;
		RECT m_rCheck;
		bool Is_Checked;

	public:
		bool SetInformaition(std::string _Name, D3DXVECTOR3 _vec3Position, std::string _Text, RECT* _Rect = nullptr, RECT* _Margin = nullptr);
		void SetText(std::string _Text);
		void SetStatus(bool Is_Checked);
		bool GetStatus();
		virtual bool IsClicked() override;
		virtual void Render() override;
	};
}