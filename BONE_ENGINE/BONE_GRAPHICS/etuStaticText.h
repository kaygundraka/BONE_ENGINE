#pragma once
#include "Common.h"
#include "etuForm.h"

namespace BONE_GRAPHICS
{
	class etuStaticText : public etuForm
	{
	private:
		std::string m_sString;

	public:
		bool SetInformaition(std::string _Name, D3DXVECTOR3 _vec3Position, int _Width, int _Height, std::string _Text);
		void SetText(std::string _Text);
		virtual void Render() override;
	};
}