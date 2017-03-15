#pragma once
#pragma once
#include "Common.h"
#include "etuForm.h"

namespace BONE_GRAPHICS
{
	class etuImage : public etuForm
	{
	private:
		float rotAngle;

	public:
		bool SetInformaition(std::string p_sName, std::string p_sImageAddress, D3DXVECTOR3 p_vec3Position, int p_iWidth,
			int p_iHeight, RECT* p_pRect, RECT* p_pMargin = nullptr);

		virtual void Render() override;
		virtual void Render(int Alpha);

		void SetRotation(float _rotAngle);

		void SetImageRect(RECT rect);
	};
}